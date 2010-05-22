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
	Listener(const QHostAddress &address, quint16 port, QObject *parent = 0);

private:
	QTcpServer m_server;
	Controller m_controller;
	const QChar SPACER;

private slots:
	void newConnection();
	void readyRead();

};

#endif // LISTENER_H
