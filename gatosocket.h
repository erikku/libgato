#ifndef GATOSOCKET_H
#define GATOSOCKET_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSocketNotifier>

#include "gatoaddress.h"

/** This class encapsulates a message-oriented bluetooth L2CAP socket. */
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

	enum Error {
		TimeoutError,
		UnknownError
	};

	State state() const;

	bool connectTo(const GatoAddress &addr, unsigned short cid);
	void close();

	/** Dequeues a pending message from the rx queue.
	 *  Doesn't block: if there are no pending messages, returns null QByteArray. */
	QByteArray receive();
	/** Adds a message to the tx queue. */
	void send(const QByteArray &pkt);

signals:
	void connected();
	void disconnected();
	void error(Error error);
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
