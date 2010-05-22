#include "Listener.h"
#include <QHostAddress>
#include <QCoreApplication>
#include <iostream>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName(QLatin1String("AnyLoader"));
	a.setOrganizationName(QLatin1String("AnyClip"));
	a.setOrganizationDomain(QLatin1String("anyclip.com"));
	QString hostAddress("0.0.0.0");
	int port = 8964;
	bool background = false;
	int c;
	while ( (c = getopt(argc, argv, "a:p:b")) != -1) {
		switch (c) {
		case 'a':
			hostAddress = optarg;
			break;
		case 'p':
			port = QString(optarg).toInt();
			break;
		case 'b':
			background = true;
			break;
		default:
			std::cerr << "Invalid option '" << c << "'." << std::endl;
			return 1;
		}
	}
	if (background) {
		pid_t pid, sid;
		pid = fork();
		if (pid < 0)
			return 1;
		else if (pid > 0)
			return 0;
		umask(0);
		sid = setsid();
		if (sid < 0)
			return 1;
		if ((chdir("/")) < 0)
			return 0;
		//HACK: Why do I have to comment this line? Why does it not bind when there's no stdin?
		//close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
	Listener listener;
	if (!listener.start(QHostAddress(hostAddress), port)) {
		std::cerr << "Could not bind." << std::endl;
		return 1;
	}
	return a.exec();
}
