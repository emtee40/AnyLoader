#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Movie.h"
#include "UploadTask.h"
#include "EncodeTask.h"
#ifdef ENABLE_RIPPING
#include "RipTask.h"
#endif
#include <QObject>
#include <QLinkedList>
#include <QStringList>

class Controller : public QObject
{
	Q_OBJECT
public:
	Controller(QObject *parent = 0);
	bool addMovie(Movie *movie);
	Movie* movieForTitle(const QString &title);
	Movie* addISO(const QString &fileName);
	bool removeMovie(Movie *movie);
	QLinkedList<Movie*> addRecursiveISOs(const QString &dirName);
	QLinkedList<Movie*> movies() const;
#ifdef ENABLE_RIPPING
	RipTask* ripTask() const;
#endif
	EncodeTask* encodeTask() const;
	UploadTask* uploadTask() const;

private:
	QLinkedList<Movie*> m_movies;
#ifdef ENABLE_RIPPING
	RipTask m_ripTask;
#endif
	EncodeTask m_encodeTask;
	UploadTask m_uploadTask;

	void runTasks(Movie *movie);

public slots:
	void runTasks();

};

#endif // CONTROLLER_H
