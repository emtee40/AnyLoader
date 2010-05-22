#include "Controller.h"
#include <QDir>
#include <QFileInfo>

Controller::Controller(QObject *parent) : QObject(parent),
#ifdef ENABLE_RIPPING
	m_ripTask(this),
#endif
	m_encodeTask(this),
	m_uploadTask(this)
{
}
void Controller::runTasks()
{
	foreach(Movie* movie, m_movies)
		runTasks(movie);
}
void Controller::runTasks(Movie *movie)
{
#ifdef ENABLE_RIPPING
	if (!m_ripTask.isRunning() && m_ripTask.canRunTask(movie))
		m_ripTask.runTask(movie);
#endif
	if (!m_encodeTask.isRunning() && m_encodeTask.canRunTask(movie))
		m_encodeTask.runTask(movie);
	if (!m_uploadTask.isRunning() && m_uploadTask.canRunTask(movie))
		m_uploadTask.runTask(movie);
}
void Controller::addMovie(Movie *movie)
{
	connect(movie, SIGNAL(statusChanged()), this, SLOT(runTasks()));
	m_movies.append(movie);
	movie->setParent(this);
	runTasks(movie);
}
Movie* Controller::addISO(const QString &fileName)
{
	Movie *movie = new Movie(Movie::titleFromISOName(QFileInfo(fileName).baseName()), fileName, this);
	addMovie(movie);
	return movie;
}
Movie* Controller::movieForTitle(const QString &title)
{
	foreach(Movie* movie, m_movies)
		if (movie->title() == title)
			return movie;
	return 0;
}
QLinkedList<Movie*> Controller::movies() const
{
	return m_movies;
}
#ifdef ENABLE_RIPPING
const RipTask* Controller::ripTask() const
{
	return &m_ripTask;
}
#endif
const EncodeTask* Controller::encodeTask() const
{
	return &m_encodeTask;
}
const UploadTask* Controller::uploadTask() const
{
	return &m_uploadTask;
}
