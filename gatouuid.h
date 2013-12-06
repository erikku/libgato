#ifndef GATOUUID_H
#define GATOUUID_H

#include <QtCore/QDebug>
#include <QtCore/QtEndian>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include "libgato_global.h"

class GatoUUIDPrivate;

struct gatouint128
{
	quint8 data[16];
};

template<>
inline LIBGATO_EXPORT gatouint128 qFromLittleEndian<gatouint128>(const uchar *src)
{
	gatouint128 dest;
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
	for (int i = 0; i < 16; i++) {
		dest.data[i] = src[i];
	}
#else
	for (int i = 0; i < 16; i++) {
		dest.data[i] = src[15 - i];
	}
#endif
	return dest;
}

template<>
inline LIBGATO_EXPORT void qToLittleEndian<gatouint128>(gatouint128 src, uchar *dest)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
	for (int i = 0; i < 16; i++) {
		dest[i] = src.data[i];
	}
#else
	for (int i = 0; i < 16; i++) {
		dest[i] = src.data[15 - i];
	}
#endif
}

class LIBGATO_EXPORT GatoUUID
{
	Q_GADGET
	Q_ENUMS(GattUuid)

public:
	enum GattUuid {
		GattGenericAccessProfile = 0x1800,
		GattGenericAttributeProfile = 0x1801,
		GattPrimaryService = 0x2800,
		GattSecondaryService = 0x2801,
		GattInclude = 0x2802,
		GattCharacteristic = 0x2803,
		GattCharacteristicExtendedProperties = 0x2900,
		GattCharacteristicUserDescription = 0x2901,
		GattClientCharacteristicConfiguration = 0x2902,
		GattServerCharacteristicConfiguration = 0x2903,
		GattCharacteristicFormat = 0x2904,
		GattCharacteristicAggregateFormat = 0x2905,
		GattDeviceName = 0x2A00,
		GattAppearance = 0x2A01,
		GattPeripheralPrivacyFlag = 0x2A02,
		GattReconnectionAddress = 0x2A03,
		GattPeripheralPreferredConnectionParameters = 0x2A04,
		GattServiceChanged = 0x2A05
	};

	GatoUUID();
	GatoUUID(GattUuid uuid);
	explicit GatoUUID(quint16 uuid);
	explicit GatoUUID(quint32 uuid);
	explicit GatoUUID(gatouint128 uuid);
	explicit GatoUUID(const QString &uuid);
	GatoUUID(const GatoUUID &o);
	~GatoUUID();

	bool isNull() const;

	int minimumSize() const;

	quint16 toUInt16(bool *ok = 0) const;
	quint32 toUInt32(bool *ok = 0) const;
	gatouint128 toUInt128() const;
	QString toString() const;

	GatoUUID& operator=(const GatoUUID& o);
	friend bool operator==(const GatoUUID &a, const GatoUUID &b);
	friend bool operator!=(const GatoUUID &a, const GatoUUID &b);

private:
	QSharedDataPointer<GatoUUIDPrivate> d;
};

LIBGATO_EXPORT QDebug operator<<(QDebug debug, const GatoUUID &uuid);

LIBGATO_EXPORT QDataStream & operator<<(QDataStream &s, const gatouint128 &u);
LIBGATO_EXPORT QDataStream & operator>>(QDataStream &s, gatouint128 &u);

LIBGATO_EXPORT uint qHash(const GatoUUID &a);

#endif // GATOUUID_H
