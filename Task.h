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
	virtual bool canRunTask(Movie &movie) = 0;
	QWidget* widget();
	bool terminated() const;
protected:
	Task(bool threaded, QObject *parent = 0);
	virtual bool executeTask(Movie &movie) = 0;
	virtual void kill() = 0;
	QFutureWatcher<bool>* watcher() const;
private:
	QFutureWatcher<bool> *m_watcher;
	bool m_terminated;
private slots:
	void finished();
public slots:
	void terminate();
signals:
	void completed(bool result);
};

#endif // TASK_H
