#ifndef MOVIE_H
#define MOVIE_H

#include <QObject>
#include <QString>
#include <QList>

class Movie : public QObject
{
	Q_OBJECT

public:
	Movie(const QString &title, QObject *parent = 0);
	Movie(const QString &title, const QString &isoLocation, QObject *parent = 0);
	Movie(const QString &title, const QString &isoLocation, const QString &mp4Location, QObject *parent = 0);

	QString title() const;
	void setTitle(const QString &title);

	QString isoLocation() const;
	void setIsoLocation(const QString &isoLocation);
	bool hasRipped() const;
	void setRipped(bool ripped);

	QString mp4Location() const;
	void setMp4Location(const QString &mp4Location);
	bool hasEncoded() const;
	void setEncoded(bool encoded);

	bool hasUploaded() const;
	void setUploaded(bool uploaded);

	quint8 videoTrack() const;
	void setVideoTrack(quint8 videoTrack);
	QList<quint8>* audioTracks();

private:
	QString m_title;

	QString m_isoLocation;
	QString m_mp4Location;

	quint8 m_videoTrack;
	QList<quint8> m_audioTracks;

	bool m_hasRipped;
	bool m_hasEncoded;
	bool m_hasUploaded;

signals:
	void statusChanged() const;
};

#endif // MOVIE_H
