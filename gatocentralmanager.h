#ifndef GATOCENTRALMANAGER_H
#define GATOCENTRALMANAGER_H

#include <QtCore/QObject>
#include "libgato_global.h"
#include "gatouuid.h"

class GatoPeripheral;
class GatoAddress;
class GatoCentralManagerPrivate;

class LIBGATO_EXPORT GatoCentralManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(GatoCentralManager)
	Q_FLAGS(PeripheralScanOptions)

public:
	explicit GatoCentralManager(QObject *parent = 0);
	~GatoCentralManager();

	enum PeripheralScanOption {
		PeripheralScanOptionActive = 1 << 0,
		PeripheralScanOptionAllowDuplicates = 1 << 1
	};
	Q_DECLARE_FLAGS(PeripheralScanOptions, PeripheralScanOption)

	GatoPeripheral *getPeripheral(const GatoAddress& address);

public slots:
	void scanForPeripherals(PeripheralScanOptions options = 0);
	void scanForPeripheralsWithServices(const QList<GatoUUID>& uuids, PeripheralScanOptions options = 0);
	void stopScan();

signals:
	void discoveredPeripheral(GatoPeripheral *peripheral, quint8 advertType, int rssi);

private slots:
	void _q_readNotify();

private:
	GatoCentralManagerPrivate *const d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GatoCentralManager::PeripheralScanOptions)

#endif // GATOCENTRALMANAGER_H
