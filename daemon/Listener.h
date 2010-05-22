#ifndef LISTENER_H
#define LISTENER_H

#include "Controller.h"
#include "TitleInformation.h"

#include <QObject>
#include <QTcpServer>

class Listener : public QObject
{
	Q_OBJECT
public:
	Listener(QObject *parent = 0);
	bool start(const QHostAddress &address, quint16 port);

private:
	QTcpServer m_server;
	Controller m_controller;
	const QChar SPACER;
	const QLatin1String MULTIENTRY;
	const QLatin1String MULTILINE;

private slots:
	void newConnection();
	void readyRead();

};

#endif // LISTENER_H
