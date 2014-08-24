#ifndef GATOPERIPHERAL_P_H
#define GATOPERIPHERAL_P_H

#include "gatoperipheral.h"
#include "gatoservice.h"
#include "gatocharacteristic.h"
#include "gatodescriptor.h"
#include "gatoattclient.h"

class GatoPeripheralPrivate : public QObject
{
	Q_OBJECT

	Q_DECLARE_PUBLIC(GatoPeripheral)

public:
	GatoPeripheralPrivate(GatoPeripheral *parent);
	~GatoPeripheralPrivate();

	GatoPeripheral *q_ptr;
	GatoAddress addr;
	QString name;
	QSet<GatoUUID> service_uuids;
	QMap<GatoHandle, GatoService> services;
	QByteArray advert_data;

	bool complete_name : 1;
	bool complete_services : 1;

	/** Maps attribute handles to service handles. */
	QMap<GatoHandle, GatoHandle> characteristic_to_service;
	QMap<GatoHandle, GatoHandle> value_to_characteristic;
	QMap<GatoHandle, GatoHandle> descriptor_to_characteristic;

	GatoAttClient *att;
	QMap<uint, GatoUUID> pending_primary_reqs;
	QMap<uint, GatoHandle> pending_characteristic_reqs;
	QMap<uint, GatoHandle> pending_characteristic_read_reqs;
	QMap<uint, GatoHandle> pending_descriptor_reqs;
	QMap<uint, GatoHandle> pending_descriptor_read_reqs;

	QMap<GatoHandle, bool> pending_set_notify;

	void parseEIRFlags(quint8 data[], int len);
	void parseEIRUUIDs(int size, bool complete, quint8 data[], int len);
	void parseName(bool complete, quint8 data[], int len);

	static GatoCharacteristic parseCharacteristicValue(const QByteArray &ba);

	static QByteArray genClientCharConfiguration(bool notification, bool indication);

	void clearServices();
	void clearServiceCharacteristics(GatoService *service);
	void clearCharacteristicDescriptors(GatoCharacteristic *characteristic);

	void finishSetNotifyOperations(const GatoCharacteristic &characteristic);

public slots:
	void handleAttConnected();
	void handleAttDisconnected();
	void handleAttAttributeUpdated(GatoHandle handle, const QByteArray &value, bool confirmed);
	void handlePrimary(uint req, const QList<GatoAttClient::AttributeGroupData>& list);
	void handlePrimaryForService(uint req, const QList<GatoAttClient::HandleInformation>& list);
	void handleCharacteristic(uint req, const QList<GatoAttClient::AttributeData> &list);
	void handleDescriptors(uint req, const QList<GatoAttClient::InformationData> &list);
	void handleCharacteristicRead(uint req, const QByteArray &value);
	void handleDescriptorRead(uint req, const QByteArray &value);
	void handleCharacteristicWrite(uint req, bool ok);
	void handleDescriptorWrite(uint req, bool ok);
};

#endif // GATOPERIPHERAL_P_H
