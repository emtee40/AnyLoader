#ifndef ENCODETASK_H
#define ENCODETASK_H

#include "Task.h"
#include <QProcess>
#include <QTime>

class EncodeTask : public Task
{
	Q_OBJECT
public:
	EncodeTask(QObject *parent = 0);
	~EncodeTask();
	bool canRunTask(const Movie *movie) const;
protected:
	bool executeTask(Movie *movie);
private:
	QProcess *m_process;
	Movie *m_movie;
private slots:
	void finished(int exitCode, QProcess::ExitStatus exitStats);
	void readyRead();
	void error();
public slots:
	void kill();
signals:
	void encodeProgress(int currentTask, int totalTasks, float percent, float currentFps, float avgFps, QTime timeRemaining);
};

#endif // ENCODETASK_H
