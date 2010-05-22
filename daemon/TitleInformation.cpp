#include "TitleInformation.h"
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>

QString TitleInformation::readTitles(const QString &location)
{
	QProcess process;
	QStringList arguments;
	arguments << "-i" << location;
	arguments << "-t" << "0";
	process.start(QString("%1/HandBrakeCLI").arg(QCoreApplication::applicationDirPath()), arguments, QIODevice::ReadOnly);
	process.waitForFinished();
	QStringList output;
	if (process.exitCode() == 0 && process.exitStatus() == QProcess::NormalExit) {
		foreach(QByteArray line, process.readAllStandardError().split('\n')) {
			QByteArray trim = line.trimmed();
			if (trim.length() > 0 && trim.at(0) == '+')
				output.append(line);
		}
		return output.join(QLatin1String("\n"));
	}
	return QString();
}
