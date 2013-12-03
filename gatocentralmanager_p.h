#ifndef GATOCENTRALMANAGER_P_H
#define GATOCENTRALMANAGER_P_H

#include <QtCore/QSocketNotifier>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include "gatocentralmanager.h"
#include "gatoaddress.h"

class GatoCentralManagerPrivate
{
	Q_DECLARE_PUBLIC(GatoCentralManager)

	GatoCentralManager *q_ptr;
	int dev_id;
	int timeout;
	int hci;
	QSocketNotifier *notifier;
	QList<GatoUUID> filter_uuids;
	hci_filter hci_nf, hci_of;
	QHash<GatoAddress, GatoPeripheral*> peripherals;

	bool scanning();
	bool openDevice();
	void closeDevice();

	void handleAdvertising(le_advertising_info *info, int rssi);
};

#endif // GATOCENTRALMANAGER_P_H
