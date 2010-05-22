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
	Movie *movie = new Movie("The Sound Of Music", "/home/zx2c4/AnyLoader/The Sound Of Music/Image - The Sound Of Music.iso");
	movie->setVideoTrack(1);
	controller.addMovie(movie);
	return a.exec();
}
