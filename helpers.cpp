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

#include "helpers.h"

GatoUUID bytearray_to_gatouuid(const QByteArray &ba)
{
	switch (ba.size()) {
	case 2:
		return GatoUUID(read_le<quint16>(ba.constData()));
	case 4:
		return GatoUUID(read_le<quint32>(ba.constData()));
	case 16:
		return GatoUUID(read_le<gatouint128>(ba.constData()));
	default:
		return GatoUUID();
	}
}

QByteArray gatouuid_to_bytearray(const GatoUUID &uuid, bool use_uuid16, bool use_uuid32)
{
	QByteArray ba;

	if (use_uuid16) {
		quint16 uuid16 = uuid.toUInt16(&use_uuid16);
		if (use_uuid16) {
			ba.resize(sizeof(quint16));
			write_le(uuid16, ba.data());
			return ba;
		}
	}
	if (use_uuid32) {
		quint32 uuid32 = uuid.toUInt32(&use_uuid32);
		if (use_uuid32) {
			ba.resize(sizeof(quint32));
			write_le(uuid32, ba.data());
			return ba;
		}
	}

	gatouint128 uuid128 = uuid.toUInt128();
	ba.resize(sizeof(gatouint128));
	Q_ASSERT(ba.size() == 16);
	write_le(uuid128, ba.data());

	return ba;
}
