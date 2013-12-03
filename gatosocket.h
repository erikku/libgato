#ifndef GATOSOCKET_H
#define GATOSOCKET_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSocketNotifier>

#include "gatoaddress.h"

class GatoSocket : public QObject
{
	Q_OBJECT
	Q_ENUMS(State)

public:
	explicit GatoSocket(QObject *parent);
	~GatoSocket();

	enum State {
		StateDisconnected,
		StateConnecting,
		StateConnected
	};

	State state() const;

	bool connectTo(const GatoAddress &addr, unsigned short cid);
	void close();

	QByteArray receive();
	void send(const QByteArray &pkt);

signals:
	void connected();
	void disconnected();
	void readyRead();

private:
	bool transmit(const QByteArray &pkt);

private slots:
	void readNotify();
	void writeNotify();

private:
	State s;
	int fd;
	QSocketNotifier *readNotifier;
	QQueue<QByteArray> readQueue;
	QSocketNotifier *writeNotifier;
	QQueue<QByteArray> writeQueue;
};

#endif // GATOSOCKET_H
