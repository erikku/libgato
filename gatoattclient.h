#ifndef GATOATTCLIENT_H
#define GATOATTCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include "gatosocket.h"
#include "gatouuid.h"

class GatoAttClient : public QObject
{
	Q_OBJECT

public:
	explicit GatoAttClient(QObject *parent = 0);
	~GatoAttClient();

	GatoSocket::State state() const;

	bool connectTo(const GatoAddress& addr);
	void close();

	struct InformationData
	{
		GatoHandle handle;
		GatoUUID uuid;
	};
	struct HandleInformation
	{
		GatoHandle start;
		GatoHandle end;
	};
	struct AttributeData
	{
		GatoHandle handle;
		QByteArray value;
	};
	struct AttributeGroupData
	{
		GatoHandle start;
		GatoHandle end;
		QByteArray value;
	};

	uint request(int opcode, const QByteArray &data, QObject *receiver, const char *member);
	uint requestExchangeMTU(quint8 client_mtu, QObject *receiver, const char *member);
	uint requestFindInformation(GatoHandle start, GatoHandle end, QObject *receiver, const char *member);
	uint requestFindByTypeValue(GatoHandle start, GatoHandle end, const GatoUUID &uuid, const QByteArray& value, QObject *receiver, const char *member);
	uint requestReadByType(GatoHandle start, GatoHandle end, const GatoUUID &uuid, QObject *receiver, const char *member);
	uint requestRead(GatoHandle handle, QObject *receiver, const char *member);
	uint requestReadByGroupType(GatoHandle start, GatoHandle end, const GatoUUID &uuid, QObject *receiver, const char *member);
	uint requestWrite(GatoHandle handle, const QByteArray &value, QObject *receiver, const char *member);
	void cancelRequest(uint id);

	void command(int opcode, const QByteArray &data);
	void commandWrite(GatoHandle handle, const QByteArray &value);

signals:
	void connected();
	void disconnected();

	void attributeUpdated(GatoHandle handle, const QByteArray &value, bool confirmed);

private:
	struct Request
	{
		uint id;
		quint8 opcode;
		QByteArray pkt;
		QObject *receiver;
		QByteArray member;
	};

	void sendARequest();
	bool handleEvent(const QByteArray &event);
	bool handleResponse(const Request& req, const QByteArray &response);

	void writeUuid16or128(QDataStream& s, const GatoUUID& uuid);

	QList<InformationData> parseInformationData(const QByteArray &data);
	QList<HandleInformation> parseHandleInformation(const QByteArray &data);
	QList<AttributeData> parseAttributeData(const QByteArray &data);
	QList<AttributeGroupData> parseAttributeGroupData(const QByteArray &data);

private slots:
	void handleSocketConnected();
	void handleSocketDisconnected();
	void handleSocketReadyRead();

	void handleServerMTU(uint req, quint8 server_mtu);

private:
	GatoSocket *socket;
	quint8 mtu;
	uint next_id;
	QQueue<Request> pending_requests;
};

#endif // GATOATTCLIENT_H
