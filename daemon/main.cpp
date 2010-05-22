#include "Listener.h"
#include <QHostAddress>
#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName(QLatin1String("AnyLoader"));
	a.setOrganizationName(QLatin1String("AnyClip"));
	a.setOrganizationDomain(QLatin1String("anyclip.com"));
	QString hostAddress("0.0.0.0");
	int port = 8964;
	QString pidFile;
	QString userName;
	int c;
	while ((c = getopt(argc, argv, "a:p:b:u:")) != -1) {
		switch (c) {
		case 'a':
			hostAddress = optarg;
			break;
		case 'p':
			port = QString(optarg).toInt();
			break;
		case 'b':
			pidFile = optarg;
			break;
		case 'u':
			userName = optarg;
			break;
		default:
			std::cerr << "Invalid option '" << c << "'." << std::endl;
			return 1;
		}
	}
	if (!pidFile.isEmpty()) {
		pid_t pid, sid;
		pid = fork();
		if (pid < 0)
			return 1;
		else if (pid > 0) {
			std::ofstream pidfile;
			pidfile.open(pidFile.toAscii());
			pidfile << QString::number(pid).toStdString() << std::endl;
			pidfile.close();
			return 0;
		}
		if (!userName.isEmpty()) {
			struct passwd *pw;
			if (!(pw = getpwnam(userName.toAscii()))) {
				std::cerr << "Couldnt find username." << std::endl;
				return 1;
			}
			if (setgid(pw->pw_gid) || setuid(pw->pw_uid)) {
				std::cerr << "Couldn't set gid or uid." << std::endl;
				return 1;
			}
		}
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
