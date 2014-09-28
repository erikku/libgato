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
	/*if (minimumSize() <= 2) {
		if (ok) *ok = true;
		return data1;
	} else {
		if (ok) *ok = false;
		return 0;
	}*/
	if (ok) *ok = true;
	return data1;
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

uint qHash(const GatoUUID &uuid, uint seed)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	return uuid.data1 ^ uuid.data2 ^ (uuid.data3 << 16)
			^ ((uuid.data4[0] << 24) | (uuid.data4[1] << 16) | (uuid.data4[2] << 8) | uuid.data4[3])
			^ ((uuid.data4[4] << 24) | (uuid.data4[5] << 16) | (uuid.data4[6] << 8) | uuid.data4[7])
			^ seed;
#else
	return qHash<QUuid>(uuid, seed);
#endif
}
