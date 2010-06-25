#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include "Task.h"
#include <QProcess>
#include <QFileInfo>
#include <QStringList>

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
	const QString m_s3Prefix;
	const QStringList m_s3BaseArguments;
	int m_currentFileIndex;
	QString m_status;
	QProcess *m_process;
	QFileInfo m_currentFile;

private slots:
	void sizeQueryFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void readStatus();
	void uploadFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
	void uploadProgress(qint64 done, qint64 total);

};

#endif // UPLOADTASK_H
