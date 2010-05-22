#include "Listener.h"
#include <QHostAddress>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName(QLatin1String("AnyLoader"));
	a.setOrganizationName(QLatin1String("AnyClip"));
	a.setOrganizationDomain(QLatin1String("anyclip.com"));
	//Controller controller;
	//Movie *movie = new Movie("The Sound Of Music", "/home/zx2c4/AnyLoader/The Sound Of Music/Image - The Sound Of Music.iso");
	//movie->setVideoTrack(1);
	//controller.addMovie(movie);
	//TitleInformation ti;
	//ti.readTitles("/home/zx2c4/AnyLoader/The Sound Of Music/Image - The Sound Of Music.iso");
	Listener listener(QHostAddress("127.0.0.1"), 1122);
	return a.exec();
}
