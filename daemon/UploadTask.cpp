#include "UploadTask.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <EncodeTarget.h>

UploadTask::UploadTask(QObject *parent) : Task(false, parent),
	m_ftp(this),
	m_currentFile(0)
{
	connect(&m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
	connect(&m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(storeUploadProgress(qint64,qint64)));
	connect(&m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(commandFinished(int,bool)));
}
UploadTask::~UploadTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool UploadTask::executeTask(Movie *movie)
{
	Q_UNUSED(movie)
	m_currentFile = 0;
	m_currentFileIndex = 0;
	m_status.clear();
	m_fileUpload = 0;
	m_ftp.connectToHost("62.219.1.20");
	m_ftp.login("title_loader", "clip0l0gy");
	queueNext();
	return true;
}
void UploadTask::queueNext()
{
	if (m_currentFile) {
		m_currentFile->close();
		delete m_currentFile;
		m_currentFile = 0;
	}
	m_status.clear();
	if (m_currentFileIndex >= currentMovie()->mp4Locations().length()) {
		m_ftp.close();
		setCompleted(true);
		currentMovie()->setUploaded(true);
		return;
	}
	m_currentFile = new QFile(currentMovie()->mp4Locations().at(m_currentFileIndex));
	m_fileUpload = m_ftp.put(m_currentFile, QFileInfo(m_currentFile->fileName()).fileName(), QFtp::Binary);
}
void UploadTask::commandFinished(int id, bool error)
{
	if (error)
		terminate();
	else if (id == m_fileUpload) {
		++m_currentFileIndex;
		queueNext();
	}
}

bool UploadTask::canRunTask(const Movie *movie) const
{
	return movie->hasEncoded() && !movie->hasUploaded();
}
void UploadTask::kill()
{
	m_ftp.abort();
	m_ftp.close();
	m_status.clear();
	if (m_currentFile) {
		m_currentFile->close();
		delete m_currentFile;
		m_currentFile = 0;
	}
}
QString UploadTask::status() const
{
	return m_status;
}
void UploadTask::storeUploadProgress(qint64 done, qint64 total)
{
	m_status = QString("%4 - %1%: %2 of %3 transferred").arg(QString::number(100.0 * (double)done / (double)total, 'g', 2), QString::number(done), QString::number(total), EncodeTarget::targets().at(m_currentFileIndex).name());
}
