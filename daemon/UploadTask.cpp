#include "UploadTask.h"
#include <QDebug>

UploadTask::UploadTask(QObject *parent) : Task(false, parent),
	m_ftp(this)
{
}
UploadTask::~UploadTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool UploadTask::executeTask(Movie *movie)
{
	m_ftp.connectToHost("62.219.1.20");
	m_ftp.login("title_loader", "clip0l0gy");
	movie->setUploaded(true);
	setCompleted(true);
	return true;
}
bool UploadTask::canRunTask(const Movie *movie) const
{
	return movie->hasEncoded() && !movie->hasUploaded();
}
void UploadTask::kill()
{
	qDebug() << "STUB: UploadTask::kill()";
}
QString UploadTask::status() const
{
	return QString();
}
