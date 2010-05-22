#include "UploadTask.h"
#include <QDebug>

UploadTask::UploadTask(QObject *parent) : Task(false, parent)
{
	qDebug() << "STUB: UploadTask constructor";
}
UploadTask::~UploadTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool UploadTask::executeTask(Movie *movie)
{
	Q_UNUSED(movie)
	qDebug() << "STUB: UploadTask::executeTask()";
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
