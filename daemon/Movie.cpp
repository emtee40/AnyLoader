#include "Movie.h"
#include "EncodeTarget.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>

#ifdef ENABLE_RIPPING
Movie::Movie(const QString &title, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(fileNameFromTitle(title, QLatin1String("Image"), QLatin1String("iso"))),
		m_videoTrack(0),
		m_audioTrack(1),
		m_hasRipped(false),
		m_hasUploaded(false)
{
	setMp4LocationDirectory(QFileInfo(m_isoLocation).path());
}
#endif
Movie::Movie(const QString &title, const QString &isoLocation, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(isoLocation),
		m_videoTrack(0),
		m_audioTrack(1),
		m_hasRipped(QFile::exists(isoLocation)),
		m_hasUploaded(false)
{

	setMp4LocationDirectory(QFileInfo(m_isoLocation).path());
}
Movie::Movie(const QString &title, const QString &isoLocation, const QString &mp4LocationDirectory, QObject *parent) : QObject(parent),
		m_title(title),
		m_isoLocation(isoLocation),
		m_videoTrack(0),
		m_audioTrack(1),
		m_hasRipped(QFile::exists(isoLocation)),
		m_hasUploaded(false)
{
	setMp4LocationDirectory(mp4LocationDirectory);
}


QString Movie::fileNameFromTitle(const QString &title, const QString &extension)
{
	QString directory = QString("%1/%2/%3").arg(QDir::homePath(), QCoreApplication::applicationName(), title);
	QDir().mkpath(directory);
	return QString("%1/%2.%3").arg(directory, title, extension);
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
QStringList Movie::mp4Locations() const
{
	return m_mp4Locations;
}
void Movie::setMp4LocationDirectory(const QString &mp4LocationDirectory)
{
	m_mp4Locations.clear();
	QDir().mkpath(mp4LocationDirectory);
	foreach (EncodeTarget target, EncodeTarget::targets()) {
		m_mp4Locations.append(QString("%1/%2%3").arg(mp4LocationDirectory, m_title, target.suffix()));
	}
	setEncoded(m_mp4Locations.length() != 1);
}
bool Movie::hasEncoded() const
{
	return m_hasEncoded;
}
void Movie::setEncoded(bool encoded)
{
	m_hasEncoded = encoded;
	foreach (QString mp4Location, m_mp4Locations)
		m_hasEncoded &= QFile::exists(mp4Location);
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
	if (m_videoTrack)
		emit statusChanged();
}
quint8 Movie::audioTrack() const
{
	return m_audioTrack;
}
void Movie::setAudioTrack(quint8 audioTrack)
{
	m_audioTrack = audioTrack;
	if (m_audioTrack)
		emit statusChanged();
}
