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

#include <QtCore/QDebug>

#include "gatoatt.h"
#include "helpers.h"

#define PROTOCOL_DEBUG 0

#define ATT_CID 4
#define ATT_PSM	31

#define ATT_DEFAULT_LE_MTU 23

enum AttOpcode {
	AttOpNone = 0,
	AttOpErrorResponse = 0x1,
	AttOpExchangeMTURequest = 0x2,
	AttOpExchangeMTUResponse = 0x3,
	AttOpFindInformationRequest = 0x4,
	AttOpFindInformationResponse = 0x5,
	AttOpFindByTypeValueRequest = 0x6,
	AttOpFindByTypeValueResponse = 0x7,
	AttOpReadByTypeRequest = 0x8,
	AttOpReadByTypeResponse = 0x9,
	AttOpReadRequest = 0xA,
	AttOpReadResponse = 0xB,
	AttOpReadBlobRequest = 0xC,
	AttOpReadBlobResponse = 0xD,
	AttOpReadMultipleRequest = 0xE,
	AttOpReadMultipleResponse = 0xF,
	AttOpReadByGroupTypeRequest = 0x10,
	AttOpReadByGroupTypeResponse = 0x11,
	AttOpWriteRequest = 0x12,
	AttOpWriteResponse = 0x13,
	AttOpWriteCommand = 0x52,
	AttOpPrepareWriteRequest = 0x16,
	AttOpPrepareWriteResponse = 0x17,
	AttOpExecuteWriteRequest = 0x18,
	AttOpExecuteWriteResponse = 0x19,
	AttOpHandleValueNotification = 0x1B,
	AttOpHandleValueIndication = 0x1D,
	AttOpHandleValueConfirmation = 0x1E,
	AttOpSignedWriteCommand = 0xD2
};

static QByteArray remove_method_signature(const char *sig)
{
	const char* bracketPosition = strchr(sig, '(');
    if (!bracketPosition || !(sig[0] >= '0' && sig[0] <= '3')) {
        qWarning("Invalid slot specification");
		return QByteArray();
    }
	return QByteArray(sig + 1, bracketPosition - 1 - sig);
}

GatoAtt::GatoAtt(QObject *parent) :
    QObject(parent), socket(new GatoSocket(this)), mtu(ATT_DEFAULT_LE_MTU), next_id(1)
{
	connect(socket, SIGNAL(connected()), SLOT(handleSocketConnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(handleSocketDisconnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(handleSocketReadyRead()));
}

GatoAtt::~GatoAtt()
{
}

GatoSocket::State GatoAtt::state() const
{
	return socket->state();
}

bool GatoAtt::connectTo(const GatoAddress &addr)
{
	return socket->connectTo(addr, ATT_CID);
}

void GatoAtt::close()
{
	socket->close();
}

uint GatoAtt::request(int opcode, const QByteArray &data, QObject *receiver, const char *member)
{
	Request req;
	req.id = next_id++;
	req.opcode = opcode;
	req.pkt = data;
	req.pkt.prepend(static_cast<char>(opcode));
	req.receiver = receiver;
	req.member = remove_method_signature(member);

	pending_requests.enqueue(req);

	if (pending_requests.size() == 1) {
		// So we can just send this request instead of waiting for others to complete
		sendARequest();
	}

	return req.id;
}

void GatoAtt::cancelRequest(uint id)
{
	QQueue<Request>::iterator it = pending_requests.begin();
	while (it != pending_requests.end()) {
		if (it->id == id) {
			it = pending_requests.erase(it);
		} else {
			++it;
		}
	}
}

uint GatoAtt::requestExchangeMTU(quint8 client_mtu, QObject *receiver, const char *member)
{
	QByteArray data(1, client_mtu);
	return request(AttOpExchangeMTURequest, data, receiver, member);
}

uint GatoAtt::requestFindInformation(GatoHandle start, GatoHandle end, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << start << end;

	return request(AttOpFindInformationRequest, data, receiver, member);
}

uint GatoAtt::requestFindByTypeValue(GatoHandle start, GatoHandle end, const GatoUUID &uuid, const QByteArray &value, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << start << end;

	bool uuid16_ok;
	quint16 uuid16 = uuid.toUInt16(&uuid16_ok);
	if (uuid16_ok) {
		s << uuid16;
	} else {
		qWarning() << "FindByTypeValue does not support UUIDs other than UUID16";
		return -1;
	}

	s << value;

	return request(AttOpFindByTypeValueRequest, data, receiver, member);
}

uint GatoAtt::requestReadByType(GatoHandle start, GatoHandle end, const GatoUUID &uuid, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << start << end;
	writeUuid16or128(s, uuid);

	return request(AttOpReadByTypeRequest, data, receiver, member);
}

uint GatoAtt::requestRead(GatoHandle handle, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << handle;

	return request(AttOpReadRequest, data, receiver, member);
}

uint GatoAtt::requestReadByGroupType(GatoHandle start, GatoHandle end, const GatoUUID &uuid, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << start << end;
	writeUuid16or128(s, uuid);

	return request(AttOpReadByGroupTypeRequest, data, receiver, member);
}

uint GatoAtt::requestWrite(GatoHandle handle, const QByteArray &value, QObject *receiver, const char *member)
{
	QByteArray data;
	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s << handle;
	s.writeRawData(value.constData(), value.length());

	return request(AttOpWriteRequest, data, receiver, member);
}

void GatoAtt::command(int opcode, const QByteArray &data)
{
	QByteArray packet = data;
	packet.prepend(static_cast<char>(opcode));

	socket->send(packet);

#if PROTOCOL_DEBUG
	qDebug() << "Wrote" << packet.size() << "bytes (command)" << packet.toHex();
#endif
}

void GatoAtt::sendARequest()
{
	if (pending_requests.isEmpty()) {
		return;
	}

	Request &req = pending_requests.head();
	socket->send(req.pkt);

#if PROTOCOL_DEBUG
	qDebug() << "Wrote" << req.pkt.size() << "bytes (request)" << req.pkt.toHex();
#endif
}

bool GatoAtt::handleEvent(const QByteArray &event)
{
	const char *data = event.constData();
	quint8 opcode = event[0];
	GatoHandle handle;

	switch (opcode) {
	case AttOpHandleValueNotification:
		handle = read_le<GatoHandle>(&data[1]);
		emit attributeUpdated(handle, event.mid(3), false);
		return true;
	case AttOpHandleValueIndication:
		handle = read_le<GatoHandle>(&data[1]);

		// Send the confirmation back
		command(AttOpHandleValueConfirmation, QByteArray());

		emit attributeUpdated(handle, event.mid(3), true);
		return true;
	default:
		return false;
	}
}

bool GatoAtt::handleResponse(const Request &req, const QByteArray &response)
{
	// If we know the request, we can provide a decoded answer
	switch (req.opcode) {
	case AttOpExchangeMTURequest:
		if (response[0] == AttOpExchangeMTUResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
										  Q_ARG(quint8, response[1]));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpExchangeMTURequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
					                      Q_ARG(quint8, response[1]));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpFindInformationRequest:
		if (response[0] == AttOpFindInformationResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::InformationData>, parseInformationData(response.mid(1))));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpFindInformationRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::InformationData>, QList<InformationData>()));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpFindByTypeValueRequest:
		if (response[0] == AttOpFindByTypeValueResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::HandleInformation>, parseHandleInformation(response.mid(1))));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpFindByTypeValueRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::HandleInformation>, QList<HandleInformation>()));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpReadByTypeRequest:
		if (response[0] == AttOpReadByTypeResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::AttributeData>, parseAttributeData(response.mid(1))));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpReadByTypeRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::AttributeData>, QList<AttributeData>()));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpReadRequest:
		if (response[0] == AttOpReadResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QByteArray, response.mid(1)));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpReadRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QByteArray, QByteArray()));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpReadByGroupTypeRequest:
		if (response[0] == AttOpReadByGroupTypeResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::AttributeGroupData>, parseAttributeGroupData(response.mid(1))));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpReadByGroupTypeRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(QList<GatoAtt::AttributeGroupData>, QList<AttributeGroupData>()));
			}
			return true;
		} else {
			return false;
		}
		break;
	case AttOpWriteRequest:
		if (response[0] == AttOpWriteResponse) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(bool, true));
			}
			return true;
		} else if (response[0] == AttOpErrorResponse && response[1] == AttOpWriteRequest) {
			if (req.receiver) {
				QMetaObject::invokeMethod(req.receiver, req.member.constData(),
				                          Q_ARG(uint, req.id),
				                          Q_ARG(bool, false));
			}
			return true;
		} else {
			return false;
		}
		break;
	default: // Otherwise just send a QByteArray.
		if (req.receiver) {
			QMetaObject::invokeMethod(req.receiver, req.member.constData(),
			                          Q_ARG(const QByteArray&, response));
		}
		return true;
	}
}

void GatoAtt::writeUuid16or128(QDataStream &s, const GatoUUID &uuid)
{
	s.setByteOrder(QDataStream::LittleEndian);

	bool uuid16_ok;
	quint16 uuid16 = uuid.toUInt16(&uuid16_ok);
	if (uuid16_ok) {
		s << uuid16;
	} else {
		s << uuid.toUInt128();
	}
}

QList<GatoAtt::InformationData> GatoAtt::parseInformationData(const QByteArray &data)
{
	const int format = data[0];
	QList<InformationData> list;
	int item_len;

	switch (format) {
	case 1:
		item_len = 2 + 2;
		break;
	case 2:
		item_len = 2 + 16;
		break;
	default:
		qWarning() << "Unknown InformationData format!";
		return list;
	}

	int items = (data.size() - 1) / item_len;
	list.reserve(items);

	int pos = 1;
	const char *s = data.constData();
	for (int i = 0; i < items; i++) {
		InformationData d;
		d.handle = read_le<GatoHandle>(&s[pos]);
		switch (format) {
		case 1:
			d.uuid = GatoUUID(read_le<quint16>(&s[pos + 2]));
			break;
		case 2:
			d.uuid = GatoUUID(read_le<gatouint128>(&s[pos + 2]));
			break;
		}

		list.append(d);

		pos += item_len;
	}

	return list;
}

QList<GatoAtt::HandleInformation> GatoAtt::parseHandleInformation(const QByteArray &data)
{
	const int item_len = 2;
	const int items = data.size() / item_len;
	QList<HandleInformation> list;
	list.reserve(items);

	int pos = 0;
	const char *s = data.constData();
	for (int i = 0; i < items; i++) {
		HandleInformation d;
		d.start = read_le<GatoHandle>(&s[pos]);
		d.end = read_le<GatoHandle>(&s[pos + 2]);
		list.append(d);

		pos += item_len;
	}

	return list;
}

QList<GatoAtt::AttributeData> GatoAtt::parseAttributeData(const QByteArray &data)
{
	const int item_len = data[0];
	const int items = (data.size() - 1) / item_len;
	QList<AttributeData> list;
	list.reserve(items);

	int pos = 1;
	const char *s = data.constData();
	for (int i = 0; i < items; i++) {
		AttributeData d;
		d.handle = read_le<GatoHandle>(&s[pos]);
		d.value = data.mid(pos + 2, item_len - 2);
		list.append(d);

		pos += item_len;
	}

	return list;
}

QList<GatoAtt::AttributeGroupData> GatoAtt::parseAttributeGroupData(const QByteArray &data)
{
	const int item_len = data[0];
	const int items = (data.size() - 1) / item_len;
	QList<AttributeGroupData> list;
	list.reserve(items);

	int pos = 1;
	const char *s = data.constData();
	for (int i = 0; i < items; i++) {
		AttributeGroupData d;
		d.start = read_le<GatoHandle>(&s[pos]);
		d.end = read_le<GatoHandle>(&s[pos + 2]);
		d.value = data.mid(pos + 4, item_len - 4);
		list.append(d);

		pos += item_len;
	}

	return list;
}

void GatoAtt::handleSocketConnected()
{
	requestExchangeMTU(ATT_DEFAULT_LE_MTU, this, SLOT(handleServerMTU(quint8)));
	emit connected();
}

void GatoAtt::handleSocketDisconnected()
{
	emit disconnected();
}

void GatoAtt::handleSocketReadyRead()
{
	QByteArray pkt = socket->receive();
	if (!pkt.isEmpty()) {
#if PROTOCOL_DEBUG
		qDebug() << "Received" << pkt.size() << "bytes" << pkt.toHex();
#endif

		// Check if it is an event
		if (handleEvent(pkt)) {
			return;
		}

		// Otherwise, if we have a request waiting, check if this answers it
		if (!pending_requests.isEmpty()) {
			if (handleResponse(pending_requests.head(), pkt)) {
				pending_requests.dequeue();
				// Proceed to next request
				if (!pending_requests.isEmpty()) {
					sendARequest();
				}
				return;
			}
		}

		qDebug() << "No idea what this packet is";
	}
}

void GatoAtt::handleServerMTU(uint req, quint8 server_mtu)
{
	Q_UNUSED(req);
	if (server_mtu != 0) {
		mtu = server_mtu;
	}
}


