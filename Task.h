#ifndef TASK_H
#define TASK_H

#include "Movie.h"
#include <QObject>
#include <QFutureWatcher>

class Task : public QObject
{
	Q_OBJECT
public:
	void runTask(Movie &movie);
	virtual bool canRunTask(const Movie &movie) const = 0;
	bool isRunning() const;

protected:
	Task(bool threaded, QObject *parent = 0);
	virtual bool executeTask(Movie &movie) = 0;
	virtual void kill() = 0;
	void setCompleted(bool result);

private:
	QFutureWatcher<bool> *m_watcher;
	bool m_isRunning;

private slots:
	void finished();

public slots:
	void terminate();

signals:
	void completed(bool result);
};

#endif // TASK_H
