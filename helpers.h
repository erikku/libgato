#ifndef HELPERS_H
#define HELPERS_H

#include <QtCore/QByteArray>
#include <QtCore/QtEndian>
#include "gatouuid.h"

template<typename T>
inline T read_le(const uchar *src)
{
	return qFromLittleEndian<T>(src);
}

template<typename T>
inline T read_le(const char *src)
{
	return qFromLittleEndian<T>(reinterpret_cast<const uchar*>(src));
}

template<typename T>
void write_le(T src, uchar *dst)
{
	qToLittleEndian<T>(src, dst);
}

template<typename T>
void write_le(T src, char *dst)
{
	qToLittleEndian<T>(src, reinterpret_cast<uchar*>(dst));
}

GatoUUID bytearray_to_gatouuid(const QByteArray &ba);
QByteArray gatouuid_to_bytearray(const GatoUUID &uuid, bool use_uuid16, bool use_uuid32);
void write_gatouuid(QDataStream &s, const GatoUUID &uuid, bool use_uuid16, bool use_uuid32);

#endif // HELPERS_H
