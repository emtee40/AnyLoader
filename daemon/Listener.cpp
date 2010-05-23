#include "Listener.h"
#include <QTcpSocket>
#include <QTextStream>
#include <QStringList>
#include <QFile>

Listener::Listener(QObject *parent) : QObject(parent),
	m_controller(this),
	SPACER('|'),
	MULTIENTRY("|||||||||||||\n"),
	MULTILINE("||||||||\n")
{
	connect(&m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}
bool Listener::start(const QHostAddress &address, quint16 port)
{
	if (m_server.isListening())
		m_server.close();
	return m_server.listen(address, port);
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
				streamer << movie->title() << SPACER << movie->hasRipped() << SPACER << movie->hasEncoded() << SPACER << movie->hasUploaded() << SPACER << movie->videoTrack() << SPACER << movie->audioTrack() << endl;
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
		} else if (call.at(0) == "setAudioTrack" && call.length() >= 3) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie) {
				streamer << "error" << endl;
				continue;
			}
			movie->setAudioTrack(call.at(2).toInt());
			streamer << "success" << endl;
		} else if (call.at(0) == "ripStatus") {
#ifdef ENABLE_RIPPING
			if (m_controller.ripTask()->isRunning())
				streamer << m_controller.ripTask()->currentMovie()->title() << SPACER << m_controller.ripTask()->status() << endl;
			else
#endif
				streamer << "error" << endl;
		} else if (call.at(0) == "terminateRip") {
#ifdef ENABLE_RIPPING
			if (m_controller.ripTask()->isRunning()) {
				m_controller.ripTask()->terminate();
				streamer << "success" << endl;
			} else
#endif
				streamer << "error" << endl;
		}
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
		} else if (call.at(0) == "startTerminatedTasks") {
			m_controller.runTasks();
			streamer << "success" << endl;
		} else if (call.at(0) == "removeMovie" && call.length() == 2) {
			movie = m_controller.movieForTitle(call.at(1));
			if (!movie) {
				streamer << "error" << endl;
				continue;
			}
			if (m_controller.removeMovie(movie))
				streamer << "success" << endl;
			else
				streamer << "error" << endl;
		} else if (call.at(0) == "isRippingEnabled") {
#ifdef ENABLE_RIPPING
			streamer << true << endl;
#else
			streamer << false << endl;
#endif
		} else {
			streamer << "error" << endl;
		}
	}
}
