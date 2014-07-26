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

#include <QtCore/QString>
#include <QtCore/QHash>

#include <bluetooth/bluetooth.h>

#include "gatoaddress.h"

struct GatoAddressPrivate : QSharedData
{
	union {
		bdaddr_t bd;
		quint64 u64;
	} addr;
};

GatoAddress::GatoAddress()
    : d(new GatoAddressPrivate)
{
	d->addr.u64 = 0;
}

GatoAddress::GatoAddress(quint64 addr)
    : d(new GatoAddressPrivate)
{
	d->addr.u64 = addr;
}

GatoAddress::GatoAddress(quint8 addr[])
    : d(new GatoAddressPrivate)
{
	d->addr.u64 = 0;
	for (int i = 0; i < 6; i++) {
		d->addr.bd.b[i] = addr[i];
	}
}

GatoAddress::GatoAddress(const QString &addr)
    : d(new GatoAddressPrivate)
{
	d->addr.u64 = 0;
	str2ba(addr.toLatin1().constData(), &d->addr.bd);
}

GatoAddress::GatoAddress(const GatoAddress &o)
    : d(o.d)
{
}

GatoAddress::~GatoAddress()
{
}

GatoAddress & GatoAddress::operator =(const GatoAddress& o)
{
	if (this != &o) {
		d = o.d;
	}
	return *this;
}

bool GatoAddress::isNull() const
{
	return toUInt64() == 0;
}

quint64 GatoAddress::toUInt64() const
{
	return d->addr.u64;
}

void GatoAddress::toUInt8Array(quint8 addr[]) const
{
	for (int i = 0; i < 6; i++) {
		addr[i] = d->addr.bd.b[i];
	}
}

QString GatoAddress::toString() const
{
	char addr[18];
	ba2str(&d->addr.bd, addr);
	return QString::fromLatin1(addr);
}

bool operator==(const GatoAddress &a, const GatoAddress &b)
{
	return a.toUInt64() == b.toUInt64();
}

uint qHash(const GatoAddress &a)
{
	return qHash(a.toUInt64());
}
