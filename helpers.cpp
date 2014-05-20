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

#include <QtCore/QDataStream>
#include "helpers.h"

static QByteArray reverse(const QByteArray &ba)
{
	int size = ba.size();
	QByteArray r(size, Qt::Uninitialized);
	for (int i = 0; i < size; i++) {
		r[i] = ba[size - i - 1];
	}
	return r;
}

GatoUUID bytearray_to_gatouuid(const QByteArray &ba)
{
	switch (ba.size()) {
	case 2:
		return GatoUUID(read_le<quint16>(ba.constData()));
	case 4:
		return GatoUUID(read_le<quint32>(ba.constData()));
	case 16:
		// For some reason, Bluetooth UUIDs use "reversed big endian" order.
		return GatoUUID(QUuid::fromRfc4122(reverse(ba)));
	default:
		return GatoUUID();
	}
}

QByteArray gatouuid_to_bytearray(const GatoUUID &uuid, bool use_uuid16, bool use_uuid32)
{
	if (use_uuid16) {
		quint16 uuid16 = uuid.toUInt16(&use_uuid16);
		if (use_uuid16) {
			QByteArray bytes(sizeof(quint16), Qt::Uninitialized);
			write_le(uuid16, bytes.data());
			return bytes;
		}
	}
	if (use_uuid32) {
		quint32 uuid32 = uuid.toUInt32(&use_uuid32);
		if (use_uuid32) {
			QByteArray bytes(sizeof(quint32), Qt::Uninitialized);
			write_le(uuid32, bytes.data());
			return bytes;
		}
	}

	return reverse(uuid.toRfc4122());
}

void write_gatouuid(QDataStream &s, const GatoUUID &uuid, bool use_uuid16, bool use_uuid32)
{
	QByteArray bytes = gatouuid_to_bytearray(uuid, use_uuid16, use_uuid32);
	s.writeRawData(bytes.constData(), bytes.size());
}
