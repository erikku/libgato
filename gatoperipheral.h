#ifndef GATOPERIPHERAL_H
#define GATOPERIPHERAL_H

#include <QtCore/QObject>
#include "libgato_global.h"
#include "gatouuid.h"
#include "gatoaddress.h"

class GatoService;
class GatoCharacteristic;
class GatoDescriptor;
class GatoPeripheralPrivate;

class LIBGATO_EXPORT GatoPeripheral : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(GatoPeripheral)
	Q_ENUMS(State)
	Q_PROPERTY(GatoAddress address READ address)
	Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
	GatoPeripheral(const GatoAddress& addr, QObject *parent = 0);
	~GatoPeripheral();

	enum State {
		StateDisconnected,
		StateConnecting,
		StateConnected
	};

	State state() const;
	GatoAddress address() const;
	QString name() const;
	QList<GatoService> services() const;

	void parseEIR(quint8 data[], int len);

public slots:
	void connectPeripheral();
	void disconnectPeripheral();
	void discoverServices();
	void discoverServices(const QList<GatoUUID>& serviceUUIDs);
	void discoverCharacteristics(const GatoService &service);
	void discoverCharacteristics(const GatoService &service, const QList<GatoUUID>& characteristicUUIDs);
	void discoverDescriptors(const GatoCharacteristic &characteristic);
	void readValue(const GatoCharacteristic &characteristic);
	void readValue(const GatoDescriptor &descriptor);
	void writeValue(const GatoCharacteristic &characteristic, const QByteArray &data);
	void writeValue(const GatoDescriptor &descriptor, const QByteArray &data);
	void setNotification(const GatoCharacteristic &characteristic, bool enabled);

signals:
	void connected();
	void disconnected();
	void nameChanged();
	void servicesDiscovered();
	void characteristicsDiscovered(const GatoService &service);
	void descriptorsDiscovered(const GatoCharacteristic &characteristic);
	void valueUpdated(const GatoCharacteristic &characteristic, const QByteArray &value);
	void descriptorValueUpdated(const GatoDescriptor &descriptor, const QByteArray &value);

private:
	GatoPeripheralPrivate *const d_ptr;
};

#endif // GATOPERIPHERAL_H
