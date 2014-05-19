#ifndef GATOUUID_H
#define GATOUUID_H

#include <QtCore/QDebug>
#include <QtCore/QtEndian>
#include <QtCore/QUuid>
#include "libgato_global.h"

class GatoUUIDPrivate;

class LIBGATO_EXPORT GatoUUID : public QUuid
{
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
	explicit GatoUUID(const QString &uuid);
	GatoUUID(const GatoUUID &o);
	GatoUUID(const QUuid &uuid);
	~GatoUUID();

	int minimumSize() const;

	quint16 toUInt16(bool *ok = 0) const;
	quint32 toUInt32(bool *ok = 0) const;
};

LIBGATO_EXPORT QDebug operator<<(QDebug debug, const GatoUUID &uuid);

LIBGATO_EXPORT uint qHash(const GatoUUID &a, uint seed = 0);

#endif // GATOUUID_H
