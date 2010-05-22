#include "Controller.h"
#include "Movie.h"
#include "DVDDrive.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName(QLatin1String("AnyLoader"));
	a.setOrganizationName(QLatin1String("AnyClip"));
	a.setOrganizationDomain(QLatin1String("anyclip.com"));
	Controller controller;
	controller.addMovie(new Movie(Movie::titleFromISOName(DVDDrive::instance()->dvdName())));
	return a.exec();
}
