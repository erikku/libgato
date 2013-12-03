#ifndef GATOSERVICE_H
#define GATOSERVICE_H

#include <QtCore/QSharedDataPointer>
#include "gatouuid.h"

class GatoServicePrivate;
class GatoCharacteristic;

class LIBGATO_EXPORT GatoService
{
	Q_GADGET

public:
	GatoService();
	GatoService(const GatoService &o);
	~GatoService();

	GatoUUID uuid() const;
	void setUuid(const GatoUUID &uuid);

	GatoHandle startHandle() const;
	void setStartHandle(GatoHandle handle);

	GatoHandle endHandle() const;
	void setEndHandle(GatoHandle handle);

	QList<GatoCharacteristic> characteristics() const;
	bool containsCharacteristic(const GatoCharacteristic& characteristic) const;
	bool containsCharacteristic(GatoHandle handle) const;
	GatoCharacteristic getCharacteristic(GatoHandle handle) const;
	void addCharacteristic(const GatoCharacteristic& characteristic);
	void removeCharacteristic(const GatoCharacteristic& characteristic);
	void clearCharacteristics();

	GatoService & operator=(const GatoService &o);

private:
	QSharedDataPointer<GatoServicePrivate> d;
};

#endif // GATOSERVICE_H
