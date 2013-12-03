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

#include <QtCore/QSharedData>

#include "gatocharacteristic.h"
#include "gatodescriptor.h"

struct GatoCharacteristicPrivate : public QSharedData
{
	GatoUUID uuid;
	GatoHandle start;
	GatoHandle end;
	GatoCharacteristic::Properties properties;
	GatoHandle value_handle;
	QMap<GatoHandle, GatoDescriptor> descriptors;
	QHash<GatoUUID, GatoHandle> desc_uuids;
};

GatoCharacteristic::GatoCharacteristic()
    : d(new GatoCharacteristicPrivate)
{
	d->start = 0;
	d->end = 0;
	d->properties = 0;
	d->value_handle = 0;
}

GatoCharacteristic::GatoCharacteristic(const GatoCharacteristic &o)
    : d(o.d)
{
}

GatoCharacteristic::~GatoCharacteristic()
{
}

bool GatoCharacteristic::isNull() const
{
	return d->start == 0 || d->end == 0 || d->uuid.isNull();
}

GatoUUID GatoCharacteristic::uuid() const
{
	return d->uuid;
}

void GatoCharacteristic::setUuid(const GatoUUID &uuid)
{
	d->uuid = uuid;
}

GatoCharacteristic::Properties GatoCharacteristic::properties() const
{
	return d->properties;
}

void GatoCharacteristic::setProperties(GatoCharacteristic::Properties props)
{
	d->properties = props;
}

GatoHandle GatoCharacteristic::startHandle() const
{
	return d->start;
}

void GatoCharacteristic::setStartHandle(GatoHandle handle)
{
	d->start = handle;
}

GatoHandle GatoCharacteristic::endHandle() const
{
	return d->end;
}

void GatoCharacteristic::setEndHandle(GatoHandle handle)
{
	d->end = handle;
}

GatoHandle GatoCharacteristic::valueHandle() const
{
	return d->value_handle;
}

void GatoCharacteristic::setValueHandle(GatoHandle handle)
{
	d->value_handle = handle;
}

QList<GatoDescriptor> GatoCharacteristic::descriptors() const
{
	return d->descriptors.values();
}

bool GatoCharacteristic::containsDescriptor(GatoUUID uuid) const
{
	return d->desc_uuids.contains(uuid);
}

bool GatoCharacteristic::containsDescriptor(GatoHandle handle) const
{
	return d->descriptors.contains(handle);
}

GatoDescriptor GatoCharacteristic::getDescriptor(GatoUUID uuid) const
{
	return d->descriptors.value(d->desc_uuids.value(uuid));
}

GatoDescriptor GatoCharacteristic::getDescriptor(GatoHandle handle) const
{
	return d->descriptors.value(handle);
}

void GatoCharacteristic::addDescriptor(const GatoDescriptor &descriptor)
{
	d->descriptors.insert(descriptor.handle(), descriptor);
	d->desc_uuids.insert(descriptor.uuid(), descriptor.handle());
}

void GatoCharacteristic::removeDescriptor(const GatoDescriptor &descriptor)
{
	d->descriptors.remove(descriptor.handle());
	d->desc_uuids.remove(descriptor.uuid());
}

void GatoCharacteristic::clearDescriptors()
{
	d->descriptors.clear();
	d->desc_uuids.clear();
}

GatoCharacteristic &GatoCharacteristic::operator=(const GatoCharacteristic &o)
{
	if (this != &o) {
		d = o.d;
	}
	return *this;
}
