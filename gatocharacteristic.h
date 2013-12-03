#ifndef GATOCHARACTERISTIC_H
#define GATOCHARACTERISTIC_H

#include <QtCore/QObject>
#include <QtCore/QSharedDataPointer>

#include "gatouuid.h"

class GatoCharacteristicPrivate;
class GatoDescriptor;

class LIBGATO_EXPORT GatoCharacteristic
{
	Q_GADGET
	Q_FLAGS(CharacteristicProperties)

public:
	enum Property {
		PropertyBroadcast = 0x1,
		PropertyRead = 0x2,
		PropertyWriteWithoutResponse = 0x4,
		PropertyWrite = 0x5,
		PropertyNotify = 0x10,
		PropertyIndicate = 0x20,
		PropertyAuthenticatedSignedWrites = 0x40,
		PropertyExtendedProperties = 0x80
	};
	Q_DECLARE_FLAGS(Properties, Property)

	GatoCharacteristic();
	GatoCharacteristic(const GatoCharacteristic &o);
	~GatoCharacteristic();

	bool isNull() const;

	GatoUUID uuid() const;
	void setUuid(const GatoUUID &uuid);

	Properties properties() const;
	void setProperties(Properties props);

	GatoHandle startHandle() const;
	void setStartHandle(GatoHandle handle);

	GatoHandle endHandle() const;
	void setEndHandle(GatoHandle handle);

	GatoHandle valueHandle() const;
	void setValueHandle(GatoHandle handle);

	QList<GatoDescriptor> descriptors() const;
	bool containsDescriptor(const GatoDescriptor& descriptor) const;
	bool containsDescriptor(GatoUUID uuid) const;
	bool containsDescriptor(GatoHandle handle) const;
	GatoDescriptor getDescriptor(GatoUUID uuid) const;
	GatoDescriptor getDescriptor(GatoHandle handle) const;
	void addDescriptor(const GatoDescriptor& descriptor);
	void removeDescriptor(const GatoDescriptor& descriptor);
	void clearDescriptors();

	GatoCharacteristic &operator=(const GatoCharacteristic &o);
	
private:
	QSharedDataPointer<GatoCharacteristicPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GatoCharacteristic::Properties)

#endif // GATOCHARACTERISTIC_H
