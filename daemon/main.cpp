#include "Listener.h"
#include <QHostAddress>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName(QLatin1String("AnyLoader"));
	a.setOrganizationName(QLatin1String("AnyClip"));
	a.setOrganizationDomain(QLatin1String("anyclip.com"));
	Listener listener(QHostAddress("127.0.0.1"), 1122);
	return a.exec();
}
