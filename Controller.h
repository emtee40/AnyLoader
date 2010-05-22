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

class Controller : public QObject
{
	Q_OBJECT
public:
	Controller(QObject *parent = 0);
private:
	QLinkedList<Movie*> m_movies;
#ifdef ENABLE_RIPPING
	RipTask m_ripTask;
#endif
	EncodeTask m_encodeTask;
	UploadTask m_uploadTask;

	void runTasks(Movie *movie);
	void addMovie(Movie *movie);

private slots:
	void runTasks();

};

#endif // CONTROLLER_H
