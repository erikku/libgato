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

#include "gatoservice.h"
#include "gatocharacteristic.h"

struct GatoServicePrivate : QSharedData
{
	GatoServicePrivate();
	GatoUUID uuid;
	GatoHandle start;
	GatoHandle end;

	QMap<GatoHandle, GatoCharacteristic> characteristics;
};

GatoService::GatoService()
    : d(new GatoServicePrivate)
{
}

GatoService::GatoService(const GatoService &o)
    : d(o.d)
{
}

GatoService::~GatoService()
{
}

GatoUUID GatoService::uuid() const
{
	return d->uuid;
}

void GatoService::setUuid(const GatoUUID &uuid)
{
	d->uuid = uuid;
}

GatoHandle GatoService::startHandle() const
{
	return d->start;
}

void GatoService::setStartHandle(GatoHandle handle)
{
	d->start = handle;
}

GatoHandle GatoService::endHandle() const
{
	return d->end;
}

void GatoService::setEndHandle(GatoHandle handle)
{
	d->end = handle;
}

QList<GatoCharacteristic> GatoService::characteristics() const
{
	return d->characteristics.values();
}

bool GatoService::containsCharacteristic(const GatoCharacteristic &characteristic) const
{
	GatoHandle char_handle = characteristic.startHandle();
	if (d->characteristics.contains(char_handle)) {
		return d->characteristics[char_handle].uuid() == characteristic.uuid();
	} else {
		return false;
	}
}

bool GatoService::containsCharacteristic(GatoHandle handle) const
{
	return d->characteristics.contains(handle);
}

GatoCharacteristic GatoService::getCharacteristic(GatoHandle handle) const
{
	return d->characteristics.value(handle);
}

void GatoService::addCharacteristic(const GatoCharacteristic &characteristic)
{
	d->characteristics.insert(characteristic.startHandle(), characteristic);
}

void GatoService::removeCharacteristic(const GatoCharacteristic &characteristic)
{
	d->characteristics.remove(characteristic.startHandle());
}

void GatoService::clearCharacteristics()
{
	d->characteristics.clear();
}

GatoService & GatoService::operator =(const GatoService &o)
{
	if (this != &o) {
		d = o.d;
	}
	return *this;
}

GatoServicePrivate::GatoServicePrivate()
    : uuid(), start(0), end(0)
{
}
