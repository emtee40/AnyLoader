#include "DVDDrive.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QStringList>

DVDDrive::DVDDrive(QObject *parent)
		: QObject(parent)
{
	m_halManager = new QDBusInterface(QLatin1String("org.freedesktop.Hal"), QLatin1String("/org/freedesktop/Hal/Manager"), QLatin1String("org.freedesktop.Hal.Manager"), QDBusConnection::systemBus(), this);
	QStringList devices = m_halManager->call(QLatin1String("FindDeviceByCapability"), QLatin1String("volume.disc")).arguments().at(0).toStringList();
	m_halDisc = 0;
	foreach (QString device, devices) {
		if (deviceAdded(device))
			break;
	}
	connect(m_halManager, SIGNAL(DeviceAdded(QString)), this, SLOT(deviceAdded(const QString&)));
	connect(m_halManager, SIGNAL(DeviceRemoved(QString)), this, SLOT(deviceRemoved(const QString&)));
}
bool DVDDrive::deviceAdded(const QString &device)
{
	QDBusInterface *old = m_halDisc;
	m_halDisc = new QDBusInterface(QLatin1String("org.freedesktop.Hal"), device, QLatin1String("org.freedesktop.Hal.Device"), QDBusConnection::systemBus(), this);
	if (m_halDisc->call(QLatin1String("GetPropertyBoolean"), QLatin1String("volume.disc.is_videodvd")).arguments().at(0).toBool()) {
		if (old)
			delete old;
		emit dvdAdded();
		return true;
	} else {
		delete m_halDisc;
		m_halDisc = old;
		return false;
	}
}
void DVDDrive::deviceRemoved(const QString &device)
{
	if (m_halDisc && m_halDisc->path() == device) {
		delete m_halDisc;
		m_halDisc = 0;
		emit dvdRemoved();
	}
}
QString DVDDrive::dvdName() const
{
	if (m_halDisc)
		return m_halDisc->call(QLatin1String("GetPropertyString"), QLatin1String("volume.label")).arguments().at(0).toString();
	else
		return QString();
}
QString DVDDrive::dvdDevice() const
{
	if (m_halDisc)
		return m_halDisc->call(QLatin1String("GetPropertyString"), QLatin1String("block.device")).arguments().at(0).toString();
	else
		return QString();
}
bool DVDDrive::dvdInserted() const
{
	return m_halDisc != 0;
}
void DVDDrive::eject()
{
	if (m_halDisc)
		QDBusInterface(QLatin1String("org.freedesktop.Hal"), m_halDisc->path(), QLatin1String("org.freedesktop.Hal.Device.Volume"), QDBusConnection::systemBus(), this).call(QLatin1String("Eject"), QStringList());
}
DVDDrive* DVDDrive::s_instance = 0;
DVDDrive* DVDDrive::instance()
{
	if (!s_instance)
		s_instance = new DVDDrive;
	return s_instance;
}
