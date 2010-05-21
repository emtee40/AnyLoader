#include "Movie.h"
#include <QFile>

Movie::Movie(const QString &title, QObject *parent) : QObject(parent),
		m_title(title),
		m_videoTrack(0),
		m_hasRipped(false),
		m_hasEncoded(false),
		m_hasUploaded(false)
{
}
Movie::Movie(const QString &title, const QString &isoLocation, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(isoLocation),
		m_videoTrack(0),
		m_hasRipped(QFile::exists(isoLocation)),
		m_hasEncoded(false),
		m_hasUploaded(false)
{
}
Movie::Movie(const QString &title, const QString &isoLocation, const QString &mp4Location, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(isoLocation),
		m_mp4Location(mp4Location),
		m_videoTrack(0),
		m_hasRipped(QFile::exists(isoLocation)),
		m_hasEncoded(QFile::exists(mp4Location)),
		m_hasUploaded(false)
{
}
Movie::Movie(const Movie &movie) : QObject(movie.parent()),
		m_title(movie.m_title),
		m_isoLocation(movie.m_isoLocation),
		m_mp4Location(movie.m_mp4Location),
		m_videoTrack(movie.m_videoTrack),
		m_audioTracks(movie.m_audioTracks),
		m_hasRipped(movie.m_hasRipped),
		m_hasEncoded(movie.m_hasEncoded),
		m_hasUploaded(movie.m_hasUploaded)
{
}

QString Movie::title() const
{
	return m_title;
}
void Movie::setTitle(const QString &title)
{
	m_title = title;
}
QString Movie::isoLocation() const
{
	return m_isoLocation;
}
void Movie::setIsoLocation(const QString &isoLocation)
{
	m_isoLocation = isoLocation;
	m_hasRipped = QFile::exists(isoLocation);
}
bool Movie::hasRipped() const
{
	return m_hasRipped;
}
void Movie::setRipped(bool ripped)
{
	m_hasRipped = ripped && QFile::exists(m_isoLocation);
	if (m_hasRipped)
		emit statusChanged();
}
QString Movie::mp4Location() const
{
	return m_mp4Location;
}
void Movie::setMp4Location(const QString &mp4Location)
{
	m_mp4Location = mp4Location;
	m_hasEncoded = QFile::exists(mp4Location);
}
bool Movie::hasEncoded() const
{
	return m_hasEncoded;
	if (m_hasEncoded)
		emit statusChanged();
}
void Movie::setEncoded(bool encoded)
{
	m_hasEncoded = encoded && QFile::exists(m_mp4Location);
}
bool Movie::hasUploaded() const
{
	return m_hasUploaded;
}
void Movie::setUploaded(bool uploaded)
{
	m_hasUploaded = uploaded;
	if (m_hasUploaded)
		emit statusChanged();
}
quint8 Movie::videoTrack() const
{
	return m_videoTrack;
}
void Movie::setVideoTrack(quint8 videoTrack)
{
	m_videoTrack = videoTrack;
}
QList<quint8>* Movie::audioTracks()
{
	return &m_audioTracks;
}
