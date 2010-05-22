#include "Listener.h"
#include <QTcpSocket>
#include <QTextStream>
#include <QStringList>
#include <QFile>

Listener::Listener(const QHostAddress &address, quint16 port, QObject *parent) : QObject(parent),
	m_controller(this),
	SPACER('|')
{
	connect(&m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	m_server.listen(address, port);
}
void Listener::newConnection()
{
	QTcpSocket *connection = m_server.nextPendingConnection();
	if (connection)
		connect(connection, SIGNAL(readyRead()), this, SLOT(readyRead()));
}
void Listener::readyRead()
{
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
	if (!socket) return;
	while (socket->canReadLine()) {
		QTextStream streamer(socket);
		QStringList call = streamer.readLine().split(SPACER);
		if (call.length() == 0) {
			socket->close();
			return;
		}
		Movie *movie = 0;
		if (call.at(0) == "getTitleInformation" && call.length() == 2) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie->hasRipped()) {
				streamer << "error" << endl;
				continue;
			}
			streamer << TitleInformation::readTitles(movie->isoLocation()) << endl;
		} else if (call.at(0) == "getTitles") {
			foreach (Movie *movie, m_controller.movies())
				streamer << movie->title() << SPACER << movie->hasRipped() << SPACER << movie->hasEncoded() << SPACER << movie->hasUploaded() << endl;
		} else if (call.at(0) == "addISO" && call.length() == 2) {
			if (!QFile::exists(call.at(1))) {
				streamer << "error" << endl;
				continue;
			}
			streamer << m_controller.addISO(call.at(1))->title() << endl;
		} else {
			streamer << "error" << endl;
		}
	}
	socket->close();
}
