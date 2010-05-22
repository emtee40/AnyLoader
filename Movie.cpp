#include "Movie.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>

#ifdef ENABLE_RIPPING
Movie::Movie(const QString &title, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(fileNameFromTitle(title, QLatin1String("Image"), QLatin1String("iso"))),
		m_mp4Location(fileNameFromTitle(title, QLatin1String("Encode"), QLatin1String("mp4"))),
		m_videoTrack(0),
		m_hasRipped(false),
		m_hasEncoded(false),
		m_hasUploaded(false)
{
}
#endif
Movie::Movie(const QString &title, const QString &isoLocation, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(isoLocation),
		m_mp4Location(fileNameFromFileName(isoLocation, QLatin1String("mp4"))),
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


QString Movie::fileNameFromTitle(const QString &title, const QString &type, const QString &extension)
{
	QString directory = QString("%1/%2/%3").arg(QDir::homePath(), QCoreApplication::applicationName(), title);
	QDir().mkpath(directory);
	return QString("%1/%2 - %3.%4").arg(directory, type, title, extension);
}
QString Movie::fileNameFromFileName(const QString &fileName, const QString &newExtension)
{
	QFileInfo info(fileName);
	return QString("%1/%2.%3").arg(info.path(), info.baseName(), newExtension);
}
QString Movie::titleFromISOName(const QString &isoName)
{
	QString name = QString(isoName).replace(QLatin1Char('_'), QLatin1Char(' ')).toLower();
	int i = 0;
	do {
		name[i] = name[i].toUpper();
		i = name.indexOf(QLatin1Char(' '), i) + 1;
	} while (i);
	return name;
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
}
void Movie::setEncoded(bool encoded)
{
	m_hasEncoded = encoded && QFile::exists(m_mp4Location);
	if (m_hasEncoded)
		emit statusChanged();
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
