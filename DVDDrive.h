#ifndef DVDDRIVE_H
#define DVDDRIVE_H

#include <QObject>
class QDBusInterface;

class DVDDrive : public QObject
{
	Q_OBJECT
public:
	DVDDrive(QObject *parent = 0);
	QString dvdName() const;
	QString dvdDevice() const;
	bool dvdInserted() const;
	static DVDDrive* instance();
private:
	QDBusInterface *m_halManager;
	QDBusInterface *m_halDisc;
	static DVDDrive *s_instance;
private slots:
	bool deviceAdded(const QString &device);
	void deviceRemoved(const QString &device);
public slots:
	void eject();
signals:
	void dvdAdded();
	void dvdRemoved();
};

#endif // DVDDRIVE_H
