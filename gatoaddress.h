#ifndef GATOADDRESS_H
#define GATOADDRESS_H

#include <QtCore/QDebug>
#include <QtCore/QSharedDataPointer>
#include "libgato_global.h"

class GatoAddressPrivate;

class LIBGATO_EXPORT GatoAddress
{
public:
	GatoAddress();
	explicit GatoAddress(quint8 addr[]);
	explicit GatoAddress(quint64 addr);
	explicit GatoAddress(const QString &addr);
	GatoAddress(const GatoAddress& o);
	~GatoAddress();

	GatoAddress& operator=(const GatoAddress& o);

	bool isNull() const;

	void toUInt8Array(quint8 addr[]) const;
	quint64 toUInt64() const;
	QString toString() const;

private:
	QSharedDataPointer<GatoAddressPrivate> d;
};

inline QDebug operator<<(QDebug debug, const GatoAddress &a)
{
	debug << a.toString().toLatin1().constData();
	return debug.space();
}

LIBGATO_EXPORT bool operator==(const GatoAddress &a, const GatoAddress &b);
LIBGATO_EXPORT uint qHash(const GatoAddress &a);

#endif // GATOADDRESS_H
