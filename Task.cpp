#include "Task.h"
#include <QtConcurrentRun>
#include <QDebug>

Task::Task(bool threaded, QObject *parent) : QObject(parent),
	m_isRunning(false)
{
	if (threaded) {
		m_watcher = new QFutureWatcher<bool>;
		m_watcher->setParent(this);
		connect(m_watcher, SIGNAL(finished()), this, SLOT(finished()));
	} else
		m_watcher = 0;
}
void Task::runTask(Movie *movie)
{
	if (!canRunTask(movie)) {
		qDebug() << this->objectName() << "is already running while attempting to run on" << movie->title();
		return;
	}
	m_isRunning = true;
	if (m_watcher)
		m_watcher->setFuture(QtConcurrent::run(this, &Task::executeTask, movie));
	else
		executeTask(movie);
}
void Task::finished()
{
	setCompleted(m_watcher->future().result());
}
void Task::terminate()
{
	kill();
	if (!m_watcher)
		setCompleted(false);
}
void Task::setCompleted(bool result)
{
	m_isRunning = false;
	emit completed(result);
}
bool Task::isRunning() const
{
	return m_isRunning;
}
