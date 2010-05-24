#ifndef MOVIE_H
#define MOVIE_H

#include <QObject>
#include <QString>
#include <QStringList>

class Movie : public QObject
{
	Q_OBJECT

public:
#ifdef ENABLE_RIPPING
	Movie(const QString &title, QObject *parent = 0);
#endif
	Movie(const QString &title, const QString &isoLocation, QObject *parent = 0);
	Movie(const QString &title, const QString &isoLocation, const QString &mp4Location, QObject *parent = 0);

	QString title() const;
	void setTitle(const QString &title);

	QString isoLocation() const;
	void setIsoLocation(const QString &isoLocation);
	bool hasRipped() const;
	void setRipped(bool ripped);

	QStringList mp4Locations() const;
	void setMp4LocationDirectory(const QString &mp4LocationDirectory);
	bool hasEncoded() const;
	void setEncoded(bool encoded);

	bool hasUploaded() const;
	void setUploaded(bool uploaded);

	quint8 videoTrack() const;
	void setVideoTrack(quint8 videoTrack);
	quint8 audioTrack() const;
	void setAudioTrack(quint8 audioTrack);

	static QString fileNameFromTitle(const QString &title, const QString &extension);
	static QString titleFromISOName(const QString &isoName);


private:
	QString m_title;

	QString m_isoLocation;
	QStringList m_mp4Locations;

	quint8 m_videoTrack;
	quint8 m_audioTrack;

	bool m_hasRipped;
	bool m_hasEncoded;
	bool m_hasUploaded;

signals:
	void trackSet() const;
};

#endif // MOVIE_H
