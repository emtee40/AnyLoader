#ifndef ENCODETASK_H
#define ENCODETASK_H

#include "Task.h"
#include "EncodeTarget.h"
#include <QProcess>
#include <QTime>
#include <QList>

class EncodeTask : public Task
{
	Q_OBJECT
public:
	EncodeTask(QObject *parent = 0);
	~EncodeTask();
	bool canRunTask(const Movie *movie) const;
	QString status() const;

protected:
	bool executeTask(Movie *movie);
	void cleanUp(bool result);

private:
	void encode(EncodeTarget target);
	void queueNext();
	QProcess *m_process;
	QString m_status;
	QList<int> m_tasks;

private slots:
	void finished(int exitCode, QProcess::ExitStatus exitStats);
	void readyRead();

public slots:
	void kill();

signals:
	void encodeProgress(int currentTask, int totalTasks, float percent, float currentFps, float avgFps, QTime timeRemaining);
};

#endif // ENCODETASK_H
