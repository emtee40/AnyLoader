#ifndef ENCODETARGET_H
#define ENCODETARGET_H

#include <QStringList>
#include <QList>

class EncodeTarget
{
public:
	EncodeTarget(const QStringList &handBrakeArguments, const QString &suffix, const QString &name);
	QStringList handBrakeArguments() const;
	QString suffix() const;
	QString name() const;
	static QList<EncodeTarget> targets();
private:
	QStringList m_handBrakeArguments;
	QString m_suffix;
	QString m_name;
};

#endif // ENCODETARGET_H
