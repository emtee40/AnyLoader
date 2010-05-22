#include "TitleInformation.h"
#include <QStringList>
#include <QRegExp>

TitleInformation::TitleInformation(QObject *parent) : QObject(parent),
	m_process(0)
{
}
void TitleInformation::readTitles(const QString &location)
{
	m_process = new QProcess(this);
	QStringList arguments;
	arguments << "-i" << location;
	arguments << "-t" << "0";
	connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(terminate()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	m_process->start(QLatin1String("./HandBrakeCLI"), arguments, QIODevice::ReadOnly);
}
void TitleInformation::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	QStringList output;
	if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
		foreach(QByteArray line, m_process->readAllStandardError().split('\n')) {
			if (line.trimmed().at(0) == '+')
				output.append(line);
		}
	}
	disconnect(m_process, 0, 0, 0);
	delete m_process;
	m_process = 0;
	emit titleInformation(output.join(QLatin1String("\n")));
}
void TitleInformation::terminate()
{
	if (m_process) {
		disconnect(m_process, 0, 0, 0);
		delete m_process;
		m_process = 0;
	}
	emit titleInformation(QLatin1String("Error."));
}
