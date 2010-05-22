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
	void addMovie(Movie *movie);
	Movie* movieForTitle(const QString &title);
	Movie* addISO(const QString &fileName);
	QLinkedList<Movie*> movies() const;
#ifdef ENABLE_RIPPING
	const RipTask* ripTask() const;
#endif
	const EncodeTask* encodeTask() const;
	const UploadTask* uploadTask() const;

private:
	QLinkedList<Movie*> m_movies;
#ifdef ENABLE_RIPPING
	RipTask m_ripTask;
#endif
	EncodeTask m_encodeTask;
	UploadTask m_uploadTask;

	void runTasks(Movie *movie);

private slots:
	void runTasks();

};

#endif // CONTROLLER_H
