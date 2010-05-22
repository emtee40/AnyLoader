#include "Listener.h"
#include <QTcpSocket>
#include <QTextStream>
#include <QStringList>
#include <QFile>

Listener::Listener(const QHostAddress &address, quint16 port, QObject *parent) : QObject(parent),
	m_controller(this),
	SPACER('|'),
	MULTIENTRY("|||||||||||||\n"),
	MULTILINE("||||||||\n")
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
	if (!socket || !socket->canReadLine()) return;
	QTextStream streamer(socket);
	while (socket->canReadLine()) {
		QStringList call = streamer.readLine().split(SPACER);
		if (call.length() == 0 || call.at(0).length() == 0) {
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
			streamer << MULTILINE << TitleInformation::readTitles(movie->isoLocation()) << endl << MULTILINE;
		} else if (call.at(0) == "getTitles") {
			if (m_controller.movies().count() == 0) {
				streamer << "error" << endl;
				continue;
			}
			streamer << MULTIENTRY;
			foreach (Movie *movie, m_controller.movies()) {
				QString audioTracks = "auto";
				if (movie->audioTracks()->length() > 0) {
					audioTracks.clear();
					for (int i = 0; i < movie->audioTracks()->length(); ++i) {
						if (!audioTracks.isEmpty())
							audioTracks += ",";
						audioTracks += QString::number(movie->audioTracks()->at(i));
					}
				}
				streamer << movie->title() << SPACER << movie->hasRipped() << SPACER << movie->hasEncoded() << SPACER << movie->hasUploaded() << SPACER << movie->videoTrack() << SPACER << audioTracks << endl;
			}
			streamer << MULTIENTRY;
		} else if (call.at(0) == "addISO" && call.length() == 2) {
			if (!QFile::exists(call.at(1))) {
				streamer << "error" << endl;
				continue;
			}
			movie = m_controller.addISO(call.at(1));
			if (movie)
				streamer << movie->title() << endl;
			else
				streamer << "error" << endl;
		} else if (call.at(0) == "addRecursiveISOs" && call.length() == 2) {
			QLinkedList<Movie*> movies = m_controller.addRecursiveISOs(call.at(1));
			if (movies.count() == 0) {
				streamer << "error" << endl;
				continue;
			}
			streamer << MULTIENTRY;
			foreach(movie, movies)
				streamer << movie->title() << endl;
			streamer << MULTIENTRY;
		} else if (call.at(0) == "setVideoTrack" && call.length() == 3) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie) {
				streamer << "error" << endl;
				continue;
			}
			movie->setVideoTrack(call.at(2).toInt());
			streamer << "success" << endl;
		} else if (call.at(0) == "setAudioTracks" && call.length() >= 3) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie) {
				streamer << "error" << endl;
				continue;
			}
			movie->audioTracks()->clear();
			if (call.at(2) != "auto") {
				for (int i = 2; i < call.length(); i++)
					movie->audioTracks()->append(call.at(i).toInt());
			}
			streamer << "success" << endl;
		}
#ifdef ENABLE_RIPPING
		else if (call.at(0) == "ripStatus") {
			if (m_controller.ripTask()->isRunning())
				streamer << m_controller.ripTask()->currentMovie()->title() << SPACER << m_controller.ripTask()->status() << endl;
			else
				streamer << "error" << endl;
		} else if (call.at(0) == "terminateRip") {
			if (m_controller.ripTask()->isRunning()) {
				m_controller.ripTask()->terminate();
				streamer << "success" << endl;
			} else
				streamer << "error" << endl;
		}
#endif
		else if(call.at(0) == "encodeStatus") {
			if (m_controller.encodeTask()->isRunning())
				streamer << m_controller.encodeTask()->currentMovie()->title() << SPACER << m_controller.encodeTask()->status() << endl;
			else
				streamer << "error" << endl;
		} else if (call.at(0) == "terminateEncode") {
			if (m_controller.encodeTask()->isRunning()) {
				m_controller.encodeTask()->terminate();
				streamer << "success" << endl;
			} else
				streamer << "error" << endl;
		} else if(call.at(0) == "uploadStatus") {
			if (m_controller.uploadTask()->isRunning())
				streamer << m_controller.uploadTask()->currentMovie()->title() << SPACER << m_controller.uploadTask()->status() << endl;
			else
				streamer << "error" << endl;
		} else if (call.at(0) == "terminateUpload") {
			if (m_controller.uploadTask()->isRunning()) {
				m_controller.uploadTask()->terminate();
				streamer << "success" << endl;
			} else
				streamer << "error" << endl;
		} else {
			streamer << "error" << endl;
		}
	}
}
