#include "RipTask.h"
#include "DVDDrive.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dvdcss/dvdcss.h>
#include <dvdread/dvd_reader.h>
#include <dvdread/dvd_udf.h>
#include <QDebug>
#include <QIODevice>
#include <QFile>

RipTask::RipTask(QObject *parent) : Task(true, parent),
		m_terminate(false)
{
}

RipTask::~RipTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
int RipTask::cmpvob(const void *p1, const void *p2)
{
	vobfile *v1 = (vobfile*)p1;
	vobfile *v2 = (vobfile*)p2;
	if (v1->start < v2->start)
		return -1;
	else if (v1->start > v2->start)
		return 1;
	else
		return 0;
}

bool RipTask::executeTask(Movie &movie)
{
	m_locker.lockForWrite();
	m_terminate = false;
	m_locker.unlock();
	bool ret = saveImageToPath(movie.isoLocation());
	if (ret)
		movie.setRipped(true);
	return ret;
}
bool RipTask::canRunTask(const Movie &movie) const
{
	Q_UNUSED(movie)
	return true;
}

bool RipTask::saveImageToPath(const QString &path)
{
	QFile file(path);
	file.open(QFile::WriteOnly);
	bool ret = saveImageToDevice(file);
	file.close();
	if (!ret)
		file.remove();
	return ret;
}

// Much of the CSS logic comes from Bryan Ford's Netsteria
// http://www.google.com/codesearch/p?hl=en&sa=N&cd=10&ct=rc#PY4_fj37fsw/uia/netsteria/dvd/read.cc&q=DVDCSS_SEEK_KEY
bool RipTask::saveImageToDevice(QIODevice &out)
{
	m_locker.lockForRead();
	if (m_terminate) {
		m_locker.unlock();
		return false;
	}
	m_locker.unlock();
	QString dvdDevice = DVDDrive::instance()->dvdDevice();
	dvd_reader_t *dvdr = DVDOpen(dvdDevice.toStdString().c_str());
	if (!dvdr) {
		qDebug() << "can't open DVD (dvdread)";
		return false;
	}
	m_locker.lockForRead();
	if (m_terminate) {
		m_locker.unlock();
		return false;
	}
	m_locker.unlock();
	// Find the extents of all the potentially-encrypted VOB files
	uint32_t discend = 0;
	vobfile vobs[100*10];
	int nvobs = 0;
	const int NBLOCKS = 16;
	char buf[DVDCSS_BLOCK_SIZE * NBLOCKS];
	for (int i = 0; i < 100; i++) {
		// Find the IFO and BUP files for this titleset,
		// just to make sure hiblock accounts for them.
		for (int j = 0; j < 2; j++) {
			m_locker.lockForRead();
			if (m_terminate) {
				m_locker.unlock();
				return false;
			}
			m_locker.unlock();
			char filename[30];
			const char *ext = j ? "BUP" : "IFO";
			if (i == 0) {
				sprintf(filename, "/VIDEO_TS/VIDEO_TS.%s", ext);
			} else {
				sprintf(filename, "/VIDEO_TS/VTS_%02d_0.%s", i, ext);
			}
			uint32_t size;
			uint32_t start = UDFFindFile(dvdr, filename, &size);
			if (start == 0 || size == 0)
				break;
			uint32_t end = start + (size + 2047) / DVDCSS_BLOCK_SIZE;
			qDebug() << ext << "at blocks" << start << "-" << end << "(size" << size << "bytes)";
			if (discend < end)
				discend = end;
		}

		// Find each VOB part for decryption
		for (int j = 0; j < 10; j++) {
			m_locker.lockForRead();
			if (m_terminate) {
				m_locker.unlock();
				return false;
			}
			m_locker.unlock();
			char filename[30];
			if (i == 0) {
				if (j > 0)
					break;  // VIDEO_TS.VOB has only 1 part
				sprintf(filename, "/VIDEO_TS/VIDEO_TS.VOB");
			} else {
				sprintf(filename, "/VIDEO_TS/VTS_%02d_%d.VOB", i, j);
			}
			uint32_t size;
			uint32_t start = UDFFindFile(dvdr, filename, &size);
			if (start == 0 || size == 0)
				break;  // No more parts in this title set
			uint32_t end = start + (size + (DVDCSS_BLOCK_SIZE - 1)) / DVDCSS_BLOCK_SIZE;

			qDebug() << "VOB at blocks" << start << "-" << end << "(size" << size << "bytes)";
			vobs[nvobs].start = start;
			vobs[nvobs].end = end;
			if (discend < end)
				discend = end;
			nvobs++;
		}
	}
	m_locker.lockForRead();
	if (m_terminate) {
		m_locker.unlock();
		return false;
	}
	m_locker.unlock();
	qsort(&vobs, nvobs, sizeof(vobfile), cmpvob);
	vobs[nvobs].start = vobs[nvobs].end = INT_MAX;

	DVDClose(dvdr);
	dvdr = NULL;

	dvdcss_t dcss = dvdcss_open((char*)dvdDevice.toStdString().c_str());
	if (!dcss) {
		qDebug() << "can't open DVD (dvdcss)";
		return false;
	}
	int blkno = 0;
	int curvob = 0;
	while (1) {
		m_locker.lockForRead();
		if (m_terminate) {
			m_locker.unlock();
			return false;
		}
		m_locker.unlock();;
		//printf("% 3d%%: block %d of %d (byte %lld of %lld)\r",
		//	   (int)((long long)blkno*100/discend), blkno, discend,
		//	   (long long)blkno*DVDCSS_BLOCK_SIZE, (long long)discend*DVDCSS_BLOCK_SIZE);
		//fflush(stdout);
		emit extractProgress(blkno, discend);

		int maxblks;
		int cssflags;
		while (blkno >= vobs[curvob].end)
			curvob++;
		if (blkno < vobs[curvob].start) {
			// We haven't yet reached the next VOB.
			// Just read without decrypting.
			cssflags = DVDCSS_NOFLAGS;
			maxblks = vobs[curvob].start - blkno;

		} else if (blkno == vobs[curvob].start) {
			// We're just starting a new VOB - re-key libdvdcss.
			// (Probably only needed per-title set, but...)
			qDebug() << "Re-keying at block" << blkno;
			int actblk = dvdcss_seek(dcss, blkno, DVDCSS_SEEK_KEY);
			if (actblk != blkno) {
				qDebug() << "error seeking in DVD with dvdcss";
				return false;
			}

			cssflags = DVDCSS_READ_DECRYPT;
			maxblks = vobs[curvob].end - blkno;

		} else if (blkno < vobs[curvob].end) {
			// We're in the middle of a VOB.  Decrypt it.
			cssflags = DVDCSS_READ_DECRYPT;
			maxblks = vobs[curvob].end - blkno;

		} else {
			qDebug() << "read past end";
			return false;
		}

		if (maxblks > NBLOCKS)
			maxblks = NBLOCKS;

		// Read some blocks
		int actblks = dvdcss_read(dcss, buf, maxblks, cssflags);
		if (actblks < 0) {
			qDebug() << "read error at block" <<  blkno;
			return false;
		}
		if ((cssflags & DVDCSS_READ_DECRYPT) && (buf[0x14] & 0x30))
			qDebug() << "block" << blkno << ": still scrambled!?";

		// Write them
		out.write(buf, (qint64)actblks * DVDCSS_BLOCK_SIZE);

		blkno += actblks;

		// XX dvdcss has a bug in libc_read():
		// on partial reads it seeks to the wrong position,
		// so we can't iterate until actblks == 0.
		if (actblks < maxblks)
			break;
	}
	emit extractProgress(blkno, discend);

	if (blkno < (int)discend) {
		qDebug() << "SHORT READ: only " << blkno << "of" << discend << "blocks copied";
		return false;
	}

	qDebug() << "Success:" << blkno << "blocks copied (" << (long long)blkno * DVDCSS_BLOCK_SIZE << ") of" << discend << "expected";
	return true;
}
void RipTask::kill()
{
	m_locker.lockForWrite();
	m_terminate = true;
	m_locker.unlock();
}

