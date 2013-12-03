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

#include "gatodescriptor.h"

struct GatoDescriptorPrivate : public QSharedData
{
	GatoUUID uuid;
	GatoHandle handle;
};

GatoDescriptor::GatoDescriptor()
    : d(new GatoDescriptorPrivate)
{
	d->handle = 0;
}

GatoDescriptor::GatoDescriptor(const GatoDescriptor &o)
    : d(o.d)
{
}

GatoDescriptor::~GatoDescriptor()
{
}

GatoUUID GatoDescriptor::uuid() const
{
	return d->uuid;
}

void GatoDescriptor::setUuid(const GatoUUID &uuid)
{
	d->uuid = uuid;
}

GatoHandle GatoDescriptor::handle() const
{
	return d->handle;
}

void GatoDescriptor::setHandle(GatoHandle handle)
{
	d->handle = handle;
}

GatoDescriptor &GatoDescriptor::operator=(const GatoDescriptor &o)
{
	if (this != &o) {
		d = o.d;
	}
	return *this;
}
