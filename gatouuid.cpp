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
#include <QtCore/QHash>
#include <bluetooth/uuid.h>

#include "gatouuid.h"

union convert128 {
	uint128_t u;
	gatouint128 g;
};

struct GatoUUIDPrivate : QSharedData
{
	bt_uuid_t uuid;
};

GatoUUID::GatoUUID()
    : d(new GatoUUIDPrivate)
{
	memset(&d->uuid, 0, sizeof(d->uuid));
}

GatoUUID::GatoUUID(GattUuid uuid)
    : d(new GatoUUIDPrivate)
{
	bt_uuid16_create(&d->uuid, uuid);
}

GatoUUID::GatoUUID(quint16 uuid)
    : d(new GatoUUIDPrivate)
{
	bt_uuid16_create(&d->uuid, uuid);
}

GatoUUID::GatoUUID(quint32 uuid)
    : d(new GatoUUIDPrivate)
{
	bt_uuid32_create(&d->uuid, uuid);
}

GatoUUID::GatoUUID(gatouint128 uuid)
    : d(new GatoUUIDPrivate)
{
	convert128 c128;
	c128.g = uuid;
	bt_uuid128_create(&d->uuid, c128.u);
}

GatoUUID::GatoUUID(const QString &uuid)
    : d(new GatoUUIDPrivate)
{
	bt_string_to_uuid(&d->uuid, uuid.toLatin1().constData());
}

GatoUUID::GatoUUID(const GatoUUID &o)
    : d(o.d)
{
}

GatoUUID::~GatoUUID()
{
}

bool GatoUUID::isNull() const
{
	return d->uuid.type == bt_uuid_t::BT_UUID_UNSPEC;
}

quint16 GatoUUID::toUInt16(bool *ok) const
{
	if (d->uuid.type == bt_uuid_t::BT_UUID16) {
		if (ok) *ok = true;
		return d->uuid.value.u16;
	} else {
		if (ok) *ok = false;
		return 0;
	}
}

quint32 GatoUUID::toUInt32(bool *ok) const
{
	if (d->uuid.type == bt_uuid_t::BT_UUID32) {
		if (ok) *ok = true;
		return d->uuid.value.u32;
	} else {
		if (ok) *ok = false;
		return 0;
	}
}

gatouint128 GatoUUID::toUInt128() const
{
	bt_uuid_t u128;
	bt_uuid_to_uuid128(&d->uuid, &u128);
	return *reinterpret_cast<gatouint128*>(&u128.value.u128);
}

QString GatoUUID::toString() const
{
	char buf[MAX_LEN_UUID_STR + 1];
	if (bt_uuid_to_string(&d->uuid, buf, MAX_LEN_UUID_STR) == 0) {
		return QString::fromAscii(buf);
	} else {
		return QString();
	}
}

GatoUUID & GatoUUID::operator =(const GatoUUID &o)
{
	if (this != &o) {
		d = o.d;
	}
	return *this;
}

bool operator==(const GatoUUID &a, const GatoUUID &b)
{
	return bt_uuid_cmp(&a.d->uuid, &b.d->uuid) == 0;
}

bool operator!=(const GatoUUID &a, const GatoUUID &b)
{
	return bt_uuid_cmp(&a.d->uuid, &b.d->uuid) != 0;
}

QDataStream & operator<<(QDataStream &s, const gatouint128 &u)
{
	if (static_cast<QSysInfo::Endian>(s.byteOrder()) == QSysInfo::ByteOrder) {
		for (int i = 0; i < 16; i++) {
			s << u.data[i];
		}
	} else {
		for (int i = 15; i >= 0; i--) {
			s << u.data[i];
		}
	}
	return s;
}

QDataStream & operator>>(QDataStream &s, gatouint128 &u)
{
	if (static_cast<QSysInfo::Endian>(s.byteOrder()) == QSysInfo::ByteOrder) {
		for (int i = 0; i < 16; i++) {
			s >> u.data[i];
		}
	} else {
		for (int i = 15; i >= 0; i--) {
			s >> u.data[i];
		}
	}
	return s;
}

uint qHash(const GatoUUID &a)
{
	gatouint128 u128 = a.toUInt128();

	uint h = u128.data[0] << 24 | u128.data[1] << 16 | u128.data[2] << 8 | u128.data[3] << 0;
	h ^= u128.data[4] << 24 | u128.data[5] << 16 | u128.data[6] << 8 | u128.data[7] << 0;
	h ^= u128.data[8] << 24 | u128.data[9] << 16 | u128.data[10] << 8 | u128.data[11] << 0;
	h ^= u128.data[12] << 24 | u128.data[13] << 16 | u128.data[14] << 8 | u128.data[15] << 0;

	return h;
}
