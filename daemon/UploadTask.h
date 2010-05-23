#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include "Task.h"
#include <QFtp>
class QFile;

class UploadTask : public Task
{
	Q_OBJECT
public:
	UploadTask(QObject *parent = 0);
	~UploadTask();
	bool canRunTask(const Movie *movie) const;
	QString status() const;

protected:
	bool executeTask(Movie *movie);
	void kill();
	void cleanUp(bool result);

private:
	void queueNext();
	QFtp m_ftp;
	int m_currentFileIndex;
	QString m_status;
	int m_fileUpload;
	int m_sizeQuery;
	QFile *m_currentFile;

private slots:
	void storeUploadProgress(qint64 done, qint64 total);
	void commandFinished(int id, bool error);
	void rawCommandReply(int replyCode, const QString &detail);

signals:
	void uploadProgress(qint64 done, qint64 total);

};

#endif // UPLOADTASK_H
