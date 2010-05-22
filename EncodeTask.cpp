#include "EncodeTask.h"
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QFile>
#include <QDebug>

EncodeTask::EncodeTask(QObject *parent) : Task(false, parent),
	m_process(0),
	m_movie(0)
{
}
EncodeTask::~EncodeTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool EncodeTask::executeTask(Movie *movie)
{
	m_movie = movie;
	m_process = new QProcess(this);
	connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error()));
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
	connect(m_process, SIGNAL(readyRead()), this, SLOT(readyRead()));
	QStringList arguments;
	arguments << "-i" << movie->isoLocation();
	arguments << "-o" << movie->mp4Location();
	arguments << "-e" << "x264";
	arguments << "-b" << "500";
	arguments << "-2" << "-T";
	arguments << "-E" << "faac";
	arguments << "-B" << "96";
	arguments << "-R" << "auto";
	arguments << "-6" << "stereo";
	arguments << "-N" << "eng" << "--native-dub";
	arguments << "-f" << "mp4";
	arguments << "-t" << QString::number(movie->videoTrack());
	if (!movie->audioTracks()->length()) {
		QString audioTracks;
		for (int i = 0; i < movie->audioTracks()->length(); ++i) {
			if (!audioTracks.isEmpty())
				audioTracks += ",";
			audioTracks += QString::number(movie->audioTracks()->at(i));
		}
		arguments << "-a" << audioTracks;
	}
	arguments << "--loose-anamorphic" << "--modulus" << "16";
	arguments << "--optimize" << "--decomb" << "--deblock" << "--denoise=\"weak\"";
	arguments << "-x" << "ref=3:mixed-refs:bframes=6:weightb:direct=auto:b-pyramid:me=umh:subme=9:analyse=all:8x8dct:trellis=1:no-fast-pskip:psy-rd=1,1";
	qDebug() << "starting handbreak with arguments:" << arguments;
	//m_process->setStandardErrorFile("/home/zx2c4/Desktop/error.log");
	//m_process->setStandardOutputFile("/home/zx2c4/Desktop/out.log");
	m_process->start(QLatin1String("./HandBrakeCLI"), arguments, QIODevice::ReadOnly);
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
	if (m_movie) {
		QFile::remove(m_movie->mp4Location());
		m_movie = 0;
	}
}
void EncodeTask::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (!m_process) return;
	if (exitCode == 0 && exitStatus == QProcess::NormalExit && QFile::exists(m_movie->mp4Location())) {
		disconnect(m_process, 0, 0, 0);
		m_process->terminate();
		m_process->deleteLater();
		m_process = 0;
		m_movie->setEncoded(true);
		m_movie = 0;
		setCompleted(true);
	}
	else
		terminate();
}
void EncodeTask::error()
{
	kill();
	setCompleted(false);
}
