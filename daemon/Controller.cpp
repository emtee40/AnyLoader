#include "Controller.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

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
bool Controller::addMovie(Movie *movie)
{
	foreach (Movie *existing, m_movies) {
		if (existing->title() == movie->title()) {
			delete movie;
			return false;
		}
	}
	connect(movie, SIGNAL(statusChanged()), this, SLOT(runTasks()));
	m_movies.append(movie);
	movie->setParent(this);
	runTasks(movie);
	return true;
}
Movie* Controller::addISO(const QString &fileName)
{
	Movie *movie = new Movie(Movie::titleFromISOName(QFileInfo(fileName).baseName()), fileName, this);
	if (addMovie(movie))
		return movie;
	else
		return 0;
}
QLinkedList<Movie*> Controller::addRecursiveISOs(const QString &dirName)
{
	QLinkedList<Movie*> movies;
	QDirIterator dir(dirName, QStringList() << "*.iso" << "*.ISO", QDir::Files | QDir::Readable, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
	while (dir.hasNext()) {
		Movie *next = addISO(dir.next());
		if (next)
			movies << next;
	}
	return movies;
}

Movie* Controller::movieForTitle(const QString &title)
{
	foreach(Movie* movie, m_movies)
		if (movie->title() == title)
			return movie;
	return 0;
}
bool Controller::removeMovie(Movie *movie)
{
	if (!m_movies.removeOne(movie))
		return false;
	if (m_ripTask.currentMovie() == movie)
		m_ripTask.terminate();
	if (m_encodeTask.currentMovie() == movie)
		m_encodeTask.terminate();
	if (m_uploadTask.currentMovie() == movie)
		m_uploadTask.terminate();
	runTasks();
	return true;
}

QLinkedList<Movie*> Controller::movies() const
{
	return m_movies;
}
#ifdef ENABLE_RIPPING
RipTask* Controller::ripTask() const
{
	return const_cast<RipTask*>(&m_ripTask);
}
#endif
EncodeTask* Controller::encodeTask() const
{
	return const_cast<EncodeTask*>(&m_encodeTask);
}
UploadTask* Controller::uploadTask() const
{
	return const_cast<UploadTask*>(&m_uploadTask);
}
