#include "UploadTask.h"
#include "EncodeTarget.h"
#include <QDebug>
#include <QProcess>

UploadTask::UploadTask(QObject *parent) : Task(false, parent),
	m_s3Prefix(QLatin1String("s3://anyloader_upload")),
	m_s3BaseArguments(QStringList() << "-c" << "/var/lib/anyloader/s3cfg"),
	m_process(0),
	m_currentFile(0)
{
}
UploadTask::~UploadTask()
{
	disconnect(this, 0, 0, 0);
	kill();
}
bool UploadTask::executeTask(Movie *movie)
{
	Q_UNUSED(movie)
	m_currentFileIndex = 0;
	m_status.clear();
	queueNext();
	return true;
}
void UploadTask::queueNext()
{
	kill();
	if (m_currentFileIndex >= currentMovie()->mp4Locations().length()) {
		setCompleted(true);
		return;
	}
	m_currentFile = QFileInfo(currentMovie()->mp4Locations().at(m_currentFileIndex));
	m_status = QString("%1 - Checking for already existing file...").arg(EncodeTarget::targets().at(m_currentFileIndex).name());
	m_process = new QProcess(this);
	connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(sizeQueryFinished(int,QProcess::ExitStatus)));
	m_process->start(QLatin1String("s3cmd"), QStringList(m_s3BaseArguments) << "ls" << QString("%1/%2").arg(m_s3Prefix, m_currentFile.fileName()));
}
void UploadTask::sizeQueryFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	disconnect(m_process, 0, 0, 0);
	if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
		bool success = false;
		long long size = -1;
		QString processOutput(m_process->readAllStandardOutput());
		if (!processOutput.isEmpty())
			size = processOutput.split(' ').at(4).toLongLong(&success);
		m_process->deleteLater();
		m_process = 0;
		if (success && m_currentFile.size() == size) {
			++m_currentFileIndex;
			queueNext();
		} else {
			m_status = QString("%1 - Starting transfer...").arg(EncodeTarget::targets().at(m_currentFileIndex).name());
			m_process = new QProcess(this);
			connect(m_process, SIGNAL(readyRead()), this, SLOT(readStatus()));
			connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(uploadFinished(int,QProcess::ExitStatus)));
			m_process->start(QLatin1String("s3cmd"), QStringList(m_s3BaseArguments) << "--progress" << "put" << m_currentFile.absoluteFilePath() << QString("%1/%2").arg(m_s3Prefix, m_currentFile.fileName()));
		}
	} else
		terminate();
}
void UploadTask::cleanUp(bool result)
{
	currentMovie()->setUploaded(result);
}
void UploadTask::uploadFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
		++m_currentFileIndex;
		queueNext();
	} else
		terminate();
}
bool UploadTask::canRunTask(const Movie *movie) const
{
	return movie->hasEncoded() && !movie->hasUploaded();
}
void UploadTask::kill()
{
	if (m_process) {
		disconnect(m_process, 0, 0, 0);
		m_process->terminate();
		m_process->deleteLater();
		m_process = 0;
	}
	m_status.clear();
}
QString UploadTask::status() const
{
	return m_status;
}
void UploadTask::readStatus()
{
	static const QRegExp pattern(QLatin1String(".*([0-9]+) of ([0-9]+).*"));
	QByteArray byteLine;
	while (!(byteLine = m_process->readLine()).isEmpty()) {
		QString line = QString(byteLine).trimmed();
		if (pattern.exactMatch(line)) {
			m_status = QString("%1 - %2").arg(EncodeTarget::targets().at(m_currentFileIndex).name(), line);
			emit uploadProgress(pattern.cap(1).toInt(), pattern.cap(2).toInt());
		}
	}
}
