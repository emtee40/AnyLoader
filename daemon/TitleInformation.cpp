#include "TitleInformation.h"
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDate>
#include <QHash>

QString TitleInformation::readTitles(const QString &location)
{
	static QHash<QString, QString> cache;
	QString cacheKey(QFileInfo(location).lastModified().toString().append(location));
	if (cache.contains(cacheKey))
		return cache.value(cacheKey);
	QProcess process;
	QStringList arguments;
	arguments << "-i" << location;
	arguments << "-t" << "0";
	arguments << "--no-dvdnav";
	process.start(QString("%1/HandBrakeCLI").arg(QCoreApplication::applicationDirPath()), arguments, QIODevice::ReadOnly);
	process.waitForFinished();
	QStringList output;
	if (process.exitCode() == 0 && process.exitStatus() == QProcess::NormalExit) {
		foreach(QByteArray line, process.readAllStandardError().split('\n')) {
			QByteArray trim = line.trimmed();
			if (trim.length() > 0 && trim.at(0) == '+')
				output.append(line);
		}
		QString out = output.join(QLatin1String("\n"));
		cache.insert(cacheKey, out);
		return out;
	}
	return QString();
}
