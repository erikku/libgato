/*
 *  libgato - A GATT/ATT library for use with Bluez
 *
 *  Copyright (C) 2013 Javier S. Pedro <maemo@javispedro.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QtCore/QDebug>

#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "gatocentralmanager_p.h"
#include "gatoperipheral.h"

GatoCentralManager::GatoCentralManager(QObject *parent) :
    QObject(parent), d_ptr(new GatoCentralManagerPrivate)
{
	Q_D(GatoCentralManager);
	d->q_ptr = this;

	// Hardcode the "default" bluetooth adapter for now.
	d->dev_id = hci_get_route(NULL);
	d->timeout = 1000;
	d->hci = -1;
	d->notifier = 0;
}

GatoCentralManager::~GatoCentralManager()
{
	Q_D(GatoCentralManager);
	if (d->scanning()) stopScan();
	delete d_ptr;
}

GatoPeripheral * GatoCentralManager::getPeripheral(const GatoAddress &address)
{
	Q_D(GatoCentralManager);

	GatoPeripheral *peripheral = d->peripherals.value(address);
	if (peripheral) {
		return peripheral;
	} else {
		peripheral = new GatoPeripheral(address, this);
		d->peripherals.insert(address, peripheral);
		return peripheral;
	}
}

void GatoCentralManager::scanForPeripherals(PeripheralScanOptions options)
{
	scanForPeripheralsWithServices(QList<GatoUUID>(), options);
}

void GatoCentralManager::scanForPeripheralsWithServices(const QList<GatoUUID> &uuids, PeripheralScanOptions options)
{
	Q_D(GatoCentralManager);

	if (d->scanning()) stopScan();

	if (!d->openDevice()) return;
	d->filter_uuids = uuids;

	quint8 filter_dup = options & PeripheralScanOptionAllowDuplicates ? 0 : 1;
	quint8 scan_type = options & PeripheralScanOptionActive ? 1 : 0;
	int rc;

	hci_le_set_scan_enable(d->hci, 0, 0, d->timeout);

	rc = hci_le_set_scan_parameters(d->hci, scan_type,
									htobs(0x0100), htobs(0x0030),
	                                0 /* Public address */,
	                                0 /* No filter ? */,
	                                d->timeout);
	if (rc < 0) {
		qErrnoWarning("LE Set scan parameters failed");
		d->closeDevice();
		return;
	}

	rc = hci_le_set_scan_enable(d->hci, 1, filter_dup, d->timeout);

	if (rc < 0) {
		qErrnoWarning("LE Set scan enable failed");
		d->closeDevice();
		return;
	}

	d->notifier = new QSocketNotifier(d->hci, QSocketNotifier::Read);
	connect(d->notifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));

	socklen_t olen = sizeof(d->hci_of);
	if (getsockopt(d->hci, SOL_HCI, HCI_FILTER, &d->hci_of, &olen) < 0) {
		qErrnoWarning("Could not get existing HCI socket options");
		return;
	}

	hci_filter_clear(&d->hci_nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &d->hci_nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &d->hci_nf);

	if (setsockopt(d->hci, SOL_HCI, HCI_FILTER, &d->hci_nf, sizeof(d->hci_nf)) < 0) {
		qErrnoWarning("Could not set HCI socket options");
		return;
	}

	// SocketNotifier will call _q_readNotify() when ready
}

void GatoCentralManager::stopScan()
{
	Q_D(GatoCentralManager);
	if (d->scanning()) {
		delete d->notifier;
		setsockopt(d->hci, SOL_HCI, HCI_FILTER, &d->hci_of, sizeof(d->hci_of));
		hci_le_set_scan_enable(d->hci, 0, 0, d->timeout);
		d->closeDevice();
	} else {
		qDebug() << "No scan to stop";
	}
	d->notifier = 0;
	d->filter_uuids.clear();
	hci_filter_clear(&d->hci_nf);
	hci_filter_clear(&d->hci_of);
}

void GatoCentralManager::_q_readNotify()
{
	Q_D(GatoCentralManager);
	unsigned char buf[HCI_MAX_EVENT_SIZE];

	// Read a full event
	int len;
	if ((len = read(d->hci, buf, sizeof(buf))) < 0) {
		if (errno != EAGAIN && errno != EINTR) {
			qErrnoWarning("Could not read HCI events");
		}
		return; // Will be notified later, probably.
	}

	int pos = HCI_EVENT_HDR_SIZE + 1;
	assert(pos < len);
	evt_le_meta_event *meta = reinterpret_cast<evt_le_meta_event*>(&buf[pos]);


	if (meta->subevent == EVT_LE_ADVERTISING_REPORT) {
		pos++; // Skip subevent field
		int num_reports = buf[pos];
		pos++; // Skip num_reports field

		assert(pos < len);

		while (num_reports > 0) {
			le_advertising_info *info = reinterpret_cast<le_advertising_info*>(&buf[pos]);
			assert(pos + LE_ADVERTISING_INFO_SIZE < len);
			assert(pos + LE_ADVERTISING_INFO_SIZE + info->length < len);
			int8_t *rssi = reinterpret_cast<int8_t*>(&buf[pos + LE_ADVERTISING_INFO_SIZE + info->length]);

			d->handleAdvertising(info, *rssi);

			pos += LE_ADVERTISING_INFO_SIZE + info->length + 1;
			num_reports--;
		}
	}
}

bool GatoCentralManagerPrivate::scanning()
{
	// If the HCI device is open for any reason, it means we're scanning.
	return hci != -1;
}

bool GatoCentralManagerPrivate::openDevice()
{
	hci = hci_open_dev(dev_id);

	if (hci == -1) {
		qErrnoWarning("Could not open device");
		return false;
	}

	return true;
}

void GatoCentralManagerPrivate::closeDevice()
{
	hci_close_dev(hci);
	hci = -1;
}

void GatoCentralManagerPrivate::handleAdvertising(le_advertising_info *info, int rssi)
{
	Q_Q(GatoCentralManager);

	qDebug() << "Advertising event type" << info->evt_type
	         << "address type" << info->bdaddr_type
	         << "data length" << info->length
	         << "rssi" << rssi;

	GatoAddress addr(info->bdaddr.b);
	GatoPeripheral *peripheral;
	QHash<GatoAddress, GatoPeripheral*>::iterator it = peripherals.find(addr);
	if (it == peripherals.end()) {
		peripheral = new GatoPeripheral(addr, q);
		peripherals.insert(addr, peripheral);
	} else {
		peripheral = *it;
	}

	if (info->length > 0) {
		peripheral->parseEIR(info->data, info->length);
	}

	bool passes_filter;
	if (filter_uuids.isEmpty()) {
		passes_filter = true;
	} else {
		passes_filter = false;
		foreach (const GatoUUID & filter_uuid, filter_uuids) {
			if (peripheral->advertisesService(filter_uuid)) {
				passes_filter = true;
				break;
			}
		}
	}

	if (passes_filter) {
		emit q->discoveredPeripheral(peripheral, rssi);
	}
}
