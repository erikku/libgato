/*
 *  libgato - A GATT/ATT library for use with Bluez
 *
 *  Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include <QtCore/QDataStream>
#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "gatouuid.h"

// Bluetooth style UUIDs are like
// {XXXXXXXX-0000-1000-8000-00805F9B34FB}

static const quint16 baseuuid_data2 = 0x0000;
static const quint16 baseuuid_data3 = 0x1000;
static const quint8 baseuuid_data4[8] = {0x80, 0x00,
                                         0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};

// Used in QUuid constructor, because it wants bytes as args instead of byte array
#define BASEUUID_DATA4_VALUES baseuuid_data4[0], baseuuid_data4[1], baseuuid_data4[2], baseuuid_data4[3], \
                              baseuuid_data4[4], baseuuid_data4[5], baseuuid_data4[6], baseuuid_data4[7]

GatoUUID::GatoUUID()
{

}

GatoUUID::GatoUUID(GattUuid uuid)
    : QUuid(uuid, baseuuid_data2, baseuuid_data3, BASEUUID_DATA4_VALUES)
{
}

GatoUUID::GatoUUID(quint16 uuid)
    : QUuid(uuid, baseuuid_data2, baseuuid_data3, BASEUUID_DATA4_VALUES)
{

}

GatoUUID::GatoUUID(quint32 uuid)
    : QUuid(uuid, baseuuid_data2, baseuuid_data3, BASEUUID_DATA4_VALUES)
{
}

GatoUUID::GatoUUID(gatouint128 uuid)
    : QUuid()
{
	quint32 tmp32;
	memcpy(&tmp32, &uuid.data[0], 4);
	data1 = qFromBigEndian<quint32>(tmp32);

	quint16 tmp16;
	memcpy(&tmp16, &uuid.data[4], 2);
	data2 = qFromBigEndian<quint16>(tmp16);

	memcpy(&tmp16, &uuid.data[6], 2);
	data3 = qFromBigEndian<quint16>(tmp16);

	memcpy(data4, &uuid.data[8], 8);
}

GatoUUID::GatoUUID(const QString &uuid)
    : QUuid(uuid)
{

}

GatoUUID::GatoUUID(const GatoUUID &o)
	: QUuid(o)
{

}

GatoUUID::GatoUUID(const QUuid &uuid)
    : QUuid(uuid)
{

}

GatoUUID::~GatoUUID()
{
}

int GatoUUID::minimumSize() const
{
	if (isNull()) {
		return 0;
	}

	if (data2 == baseuuid_data2 && data3 == baseuuid_data3 &&
	        memcmp(data4, baseuuid_data4, sizeof(baseuuid_data4)) == 0) {
		if (data1 & 0xFFFF0000) {
			return 4;
		} else {
			return 2;
		}
	}

	return 16;
}

quint16 GatoUUID::toUInt16(bool *ok) const
{
	if (minimumSize() <= 2) {
		if (ok) *ok = true;
		return data1;
	} else {
		if (ok) *ok = false;
		return 0;
	}
}

quint32 GatoUUID::toUInt32(bool *ok) const
{
	if (minimumSize() <= 4) {
		if (ok) *ok = true;
		return data1;
	} else {
		if (ok) *ok = false;
		return 0;
	}
}

gatouint128 GatoUUID::toUInt128() const
{
	gatouint128 uuid;

	quint32 tmp32 = qToBigEndian<quint32>(data1);
	memcpy(&uuid.data[0], &tmp32, 4);

	quint16 tmp16 = qToBigEndian<quint16>(data2);
	memcpy(&uuid.data[4], &tmp16, 2);

	tmp16 = qToBigEndian<quint16>(data3);
	memcpy(&uuid.data[6], &tmp16, 2);

	memcpy(&uuid.data[8], data4, 8);

	return uuid;
}

QDebug operator<<(QDebug debug, const GatoUUID &uuid)
{
	quint16 uuid16;
	quint32 uuid32;
	bool ok = false;

	uuid16 = uuid.toUInt16(&ok);
	if (ok) {
		debug.nospace() << "0x" << hex << uuid16 << dec;
		return debug.space();
	}

	uuid32 = uuid.toUInt32(&ok);
	if (ok) {
		debug.nospace() << "0x" << hex << uuid32 << dec;
		return debug.space();
	}

	debug.nospace() << uuid.toString().toLatin1().constData();
    return debug.space();
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
