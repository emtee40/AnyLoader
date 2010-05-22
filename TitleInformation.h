#ifndef TITLEINFORMATION_H
#define TITLEINFORMATION_H

#include <QObject>
#include <QProcess>

class TitleInformation : public QObject
{
	Q_OBJECT
public:
	TitleInformation(QObject *parent = 0);
	void readTitles(const QString &location);

private:
	QProcess *m_process;

signals:
	void titleInformation(const QString &information);

private slots:
	void finished(int exitCode, QProcess::ExitStatus exitStatus);

public slots:
	void terminate();


};

#endif // TITLEINFORMATION_H
