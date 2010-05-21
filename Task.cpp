#include "Task.h"
#include <QtConcurrentRun>

Task::Task(bool threaded, QObject *parent) : QObject(parent),
	m_terminated(false)
{
	if (threaded) {
		m_watcher = new QFutureWatcher<bool>;
		m_watcher->setParent(this);
		connect(m_watcher, SIGNAL(finished()), this, SLOT(jobFinished()));
	} else
		m_watcher = 0;
}
void Task::runTask(Movie &movie)
{
	if (!canRunTask(movie))
		return;
	m_terminated = false;
	if (m_watcher)
		m_watcher->setFuture(QtConcurrent::run(this, &Task::executeTask, movie));
	else
		executeTask(movie);
}
void Task::finished()
{
	if (!m_terminated)
		emit completed(m_watcher->future().result());
}
QFutureWatcher<bool>* Task::watcher() const
{
	return m_watcher;
}
void Task::terminate()
{
	m_terminated = true;
	kill();
	emit completed(false);
}
bool Task::terminated() const
{
	return m_terminated;
}
