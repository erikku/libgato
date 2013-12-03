#ifndef GATODESCRIPTOR_H
#define GATODESCRIPTOR_H

#include <QtCore/QObject>
#include <QtCore/QSharedDataPointer>

#include "gatouuid.h"

class GatoDescriptorPrivate;

class LIBGATO_EXPORT GatoDescriptor
{
	Q_GADGET

public:
	GatoDescriptor();
	GatoDescriptor(const GatoDescriptor &o);
	~GatoDescriptor();

	GatoUUID uuid() const;
	void setUuid(const GatoUUID &uuid);

	GatoHandle handle() const;
	void setHandle(GatoHandle handle);

	GatoDescriptor &operator=(const GatoDescriptor &o);

private:
	QSharedDataPointer<GatoDescriptorPrivate> d;
};

#endif // GATODESCRIPTOR_H
