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
			if (!movie || !movie->hasRipped()) {
				streamer << "error" << endl;
				continue;
			}
			streamer << TitleInformation::readTitles(movie->isoLocation()) << endl;
		} else if (call.at(0) == "getTitles") {
			foreach (Movie *movie, m_controller.movies()) {
				QString audioTracks;
				if (!movie->audioTracks()->length()) {
					for (int i = 0; i < movie->audioTracks()->length(); ++i) {
						if (!audioTracks.isEmpty())
							audioTracks += ",";
						audioTracks += QString::number(movie->audioTracks()->at(i));
					}
				} else
					audioTracks = "auto";
				streamer << movie->title() << SPACER << movie->hasRipped() << SPACER << movie->hasEncoded() << SPACER << movie->hasUploaded() << SPACER << movie->videoTrack() << SPACER << audioTracks << endl;
			}
		} else if (call.at(0) == "addISO" && call.length() == 2) {
			if (!QFile::exists(call.at(1))) {
				streamer << "error" << endl;
				continue;
			}
			streamer << m_controller.addISO(call.at(1))->title() << endl;
		} else if (call.at(0) == "setVideoTrack" && call.length() == 3) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie) {
				streamer << "error" << endl;
				continue;
			}
			movie->setVideoTrack(call.at(2).toInt());
			streamer << "success" << endl;
		}
#ifdef ENABLE_RIPPING
		else if (call.at(0) == "ripStatus") {
			if (m_controller.ripTask()->isRunning())
				streamer << m_controller.ripTask()->currentMovie()->title() << SPACER << m_controller.ripTask()->status() << endl;
			else
				streamer << "not running" << endl;
		}
#endif
		else if(call.at(0) == "encodeStatus") {
			if (m_controller.encodeTask()->isRunning())
				streamer << m_controller.encodeTask()->currentMovie()->title() << SPACER << m_controller.encodeTask()->status() << endl;
			else
				streamer << "not running" << endl;
		} else if(call.at(0) == "uploadStatus") {
			if (m_controller.uploadTask()->isRunning())
				streamer << m_controller.uploadTask()->currentMovie()->title() << SPACER << m_controller.uploadTask()->status() << endl;
			else
				streamer << "not running" << endl;
		} else {
			streamer << "error" << endl;
		}
	}
	socket->close();
}
