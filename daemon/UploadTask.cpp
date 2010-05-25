#include "UploadTask.h"
#include "EncodeTarget.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFtp>

UploadTask::UploadTask(QObject *parent) : Task(false, parent),
	m_ftp(0),
	m_sizeQuery(0),
	m_currentFile(0)
{
}
UploadTask::~UploadTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool UploadTask::executeTask(Movie *movie)
{
	Q_UNUSED(movie)
	m_ftp = new QFtp(this);
	connect(m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
	connect(m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)), this, SLOT(storeUploadProgress(qint64,qint64)));
	connect(m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(commandFinished(int,bool)));
	connect(m_ftp, SIGNAL(rawCommandReply(int,QString)), this, SLOT(rawCommandReply(int,QString)));
	m_currentFile = 0;
	m_currentFileIndex = 0;
	m_status.clear();
	m_fileUpload = 0;
	m_ftp->connectToHost("62.219.1.20");
	m_ftp->login("title_loader", "clip0l0gy");
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
		if (m_ftp) {
			disconnect(m_ftp, 0, 0, 0);
			m_ftp->deleteLater();
			m_ftp = 0;
		}
		setCompleted(true);
		return;
	}
	m_currentFile = new QFile(currentMovie()->mp4Locations().at(m_currentFileIndex));
	m_status = QString("%1 - Checking for already existing file...").arg(EncodeTarget::targets().at(m_currentFileIndex).name());
	m_sizeQuery = m_ftp->rawCommand(QString("SIZE %1").arg(QFileInfo(m_currentFile->fileName()).fileName()));
}
void UploadTask::rawCommandReply(int replyCode, const QString &detail)
{
	if (replyCode == 213 && m_ftp->currentId() == m_sizeQuery) {
		bool success;
		long long size = detail.toLongLong(&success);
		if (success && m_currentFile->size() == size) {
			++m_currentFileIndex;
			queueNext();
		} else
			commandFinished(m_sizeQuery, true);
	}
}
void UploadTask::cleanUp(bool result)
{
	currentMovie()->setUploaded(result);
}
void UploadTask::commandFinished(int id, bool error)
{
	if (error && id == m_sizeQuery) {
		m_status = QString("%1 - Starting transfer...").arg(EncodeTarget::targets().at(m_currentFileIndex).name());
		m_fileUpload = m_ftp->put(m_currentFile, QFileInfo(m_currentFile->fileName()).fileName(), QFtp::Binary);
	}
	else if (error)
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
	if (m_ftp) {
		disconnect(m_ftp, 0, 0, 0);
		m_ftp->deleteLater();
		m_ftp = 0;
	}
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
