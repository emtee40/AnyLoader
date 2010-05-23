#include "EncodeTask.h"
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

EncodeTask::EncodeTask(QObject *parent) : Task(false, parent),
	m_process(0)
{
}
EncodeTask::~EncodeTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
void EncodeTask::encode(EncodeTarget target)
{
	qDebug() << "Encoding" << target.name() << "for" << currentMovie()->title();
	m_process = new QProcess(this);
	connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(terminate()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	connect(m_process, SIGNAL(readyRead()), this, SLOT(readyRead()));
	QStringList arguments;
	arguments << "-i" << currentMovie()->isoLocation();
	arguments << "-o" << currentMovie()->mp4Locations().at(m_tasks.at(0));
	arguments << target.handBrakeArguments();
	arguments << "-t" << QString::number(currentMovie()->videoTrack());
	/*if (!movie->audioTracks()->length()) {
		QString audioTracks;
		for (int i = 0; i < movie->audioTracks()->length(); ++i) {
			if (!audioTracks.isEmpty())
				audioTracks += ",";
			audioTracks += QString::number(movie->audioTracks()->at(i));
		}
		arguments << "-a" << audioTracks;
	} else
		arguments << "-N" << "eng" << "--native-dub";
	*/
	qDebug() << "Starting handbreak with arguments:" << arguments;
	//m_process->setStandardErrorFile("/home/zx2c4/Desktop/error.log");
	//m_process->setStandardOutputFile("/home/zx2c4/Desktop/out.log");
	m_process->start(QString("%1/HandBrakeCLI").arg(QCoreApplication::applicationDirPath()), arguments, QIODevice::ReadOnly);
}

bool EncodeTask::executeTask(Movie *movie)
{
	m_tasks.clear();
	for (int i = 0; i < movie->mp4Locations().length(); ++i) {
		if (!QFile::exists(movie->mp4Locations().at(i)))
			m_tasks.append(i);
	}
	queueNext();
	return true;
}
bool EncodeTask::canRunTask(const Movie *movie) const
{
	return movie->hasRipped() && !movie->hasEncoded() && !movie->hasUploaded() && movie->videoTrack() != 0;
}
void EncodeTask::readyRead()
{
	static const QRegExp percentLinePattern(QLatin1String("^Encoding: task ([0-9]*) of ([0-9]*), ([0-9]*\\.[0-9]*) % (\\(([0-9]*\\.[0-9]*) fps, avg ([0-9]*\\.[0-9]*) fps, ETA ([0-9]{2})h([0-9]{2})m([0-9]{2})s\\))?"));
	static const QRegExp lessPercentLinePattern(QLatin1String("^Encoding: task ([0-9]*) of ([0-9]*), ([0-9]*\\.[0-9]*) %"));
	QByteArray byteLine;
	while (!(byteLine = m_process->readLine()).isEmpty()) {
		QString line = QString(byteLine).trimmed();
		m_status = QString("%1 - %2").arg(EncodeTarget::targets().at(m_tasks.at(0)).name(), line);
		if (percentLinePattern.exactMatch(line)) {
			int currentTask = percentLinePattern.cap(1).toInt();
			int totalTasks = percentLinePattern.cap(2).toInt();
			float percent = percentLinePattern.cap(3).toFloat();
			float currentFps = percentLinePattern.cap(5).toFloat();
			float avgFps = percentLinePattern.cap(6).toFloat();
			QTime timeRemaining(percentLinePattern.cap(7).toInt(), percentLinePattern.cap(8).toInt(), percentLinePattern.cap(9).toInt());
			emit encodeProgress(currentTask, totalTasks, percent, currentFps, avgFps, timeRemaining);
		} else if(lessPercentLinePattern.exactMatch(line)) {
			int currentTask = lessPercentLinePattern.cap(1).toInt();
			int totalTasks = lessPercentLinePattern.cap(2).toInt();
			float percent = lessPercentLinePattern.cap(3).toFloat();
			emit encodeProgress(currentTask, totalTasks, percent, -1, -1, QTime());
		}
	}
}
void EncodeTask::kill()
{
	if (m_process) {
		disconnect(m_process, 0, 0, 0);
		m_process->terminate();
		m_process->deleteLater();
		m_process = 0;
	}
	m_status.clear();
	m_tasks.clear();
	if (currentMovie() && m_tasks.length() != 0) {
		QFile::remove(currentMovie()->mp4Locations().at(m_tasks.at(0)));
	}
}
void EncodeTask::queueNext()
{
	if (m_tasks.length() == 0) {
		setCompleted(true);
		currentMovie()->setEncoded(true);
	} else
		encode(EncodeTarget::targets().at(m_tasks.at(0)));
}
void EncodeTask::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	m_status.clear();
	if (!m_process) return;
	if (exitCode == 0 && exitStatus == QProcess::NormalExit && QFile::exists(currentMovie()->mp4Locations().at(m_tasks.at(0)))) {
		disconnect(m_process, 0, 0, 0);
		m_process->terminate();
		m_process->deleteLater();
		m_process = 0;
		m_tasks.removeFirst();
		queueNext();
	}
	else
		terminate();
}
QString EncodeTask::status() const
{
	return m_status;
}
