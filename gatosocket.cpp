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

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include "gatosocket.h"

GatoSocket::GatoSocket(QObject *parent)
    : QObject(parent), s(StateDisconnected), fd(-1)
{
}

GatoSocket::~GatoSocket()
{
	if (s != StateDisconnected) {
		close();
	}
}

GatoSocket::State GatoSocket::state() const
{
	return s;
}

bool GatoSocket::connectTo(const GatoAddress &addr, unsigned short cid)
{
	if (s != StateDisconnected) {
		qWarning() << "Already connecting or connected";
		return false;
	}

	fd = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (fd == -1) {
		qErrnoWarning("Could not create L2CAP socket");
		return false;
	}

	s = StateConnecting;

	readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
	writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);
	connect(readNotifier, SIGNAL(activated(int)), SLOT(readNotify()));
	connect(writeNotifier, SIGNAL(activated(int)), SLOT(writeNotify()));

	struct sockaddr_l2 l2addr;
	memset(&l2addr, 0, sizeof(l2addr));

	l2addr.l2_family = AF_BLUETOOTH;
	l2addr.l2_cid = htobs(cid);
#ifdef BDADDR_LE_PUBLIC
	l2addr.l2_bdaddr_type = BDADDR_LE_PUBLIC; // TODO
#endif
	addr.toUInt8Array(l2addr.l2_bdaddr.b);

	int err = ::connect(fd, reinterpret_cast<sockaddr*>(&l2addr), sizeof(l2addr));
	if (err == -1 && errno != EINPROGRESS) {
		qErrnoWarning("Could not connect to L2CAP socket");
		close();
		return false;
	}

	return true;
}

void GatoSocket::close()
{
	if (s != StateDisconnected) {
		// TODO We do not flush the writeQueue, but rather drop all data.
		delete readNotifier;
		delete writeNotifier;
		readQueue.clear();
		writeQueue.clear();
		fd = -1;
		s = StateDisconnected;
		emit disconnected();
	}
}

QByteArray GatoSocket::receive()
{
	if (readQueue.isEmpty()) {
		return QByteArray();
	} else {
		return readQueue.dequeue();
	}
}

void GatoSocket::send(const QByteArray &pkt)
{
	if (s == StateConnected && writeQueue.isEmpty()) {
		if (transmit(pkt)) {
			// Packet transmited succesfully without any queuing
			return;
		}
	}

	writeQueue.enqueue(pkt);
	writeNotifier->setEnabled(true);
}

bool GatoSocket::transmit(const QByteArray &pkt)
{
	int written = ::write(fd, pkt.constData(), pkt.size());
	if (written < 0) {
		qErrnoWarning("Could not write to L2 socket");
		close();
		return false;
	} else if (written < pkt.size()) {
		qWarning("Could not write full packet to L2 socket");
		return true;
	} else {
		return true;
	}
}

void GatoSocket::readNotify()
{
	QByteArray buf;
	buf.resize(1024); // Max packet size

	int read = ::read(fd, buf.data(), buf.size());
	if (read < 0) {
		qErrnoWarning("Could not read from L2 socket");
		close();
		return;
	} else if (read == 0) {
		return;
	}

	buf.resize(read);

	readQueue.enqueue(buf);

	if (readQueue.size() == 1) {
		emit readyRead();
	}
}

void GatoSocket::writeNotify()
{
	if (s == StateConnecting) {
		int soerror = 0;
		socklen_t len = sizeof(soerror);
		if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerror, &len) != 0) {
			// An error while reading the error
			qErrnoWarning("Could not get L2 socket options");
			close();
			return;
		}
		if (soerror != 0) {
			qWarning() << "Could not connect to L2 socket: " << strerror(soerror);
			close();
			return;
		}

		s = StateConnected;
		emit connected();

		bt_security bt_sec;
		len = sizeof(bt_sec);
		if (::getsockopt(fd, SOL_BLUETOOTH, BT_SECURITY, &bt_sec, &len) == 0) {
			qDebug() << "Established a bluetooth channel with security level " << bt_sec.level;
		}
	} else if (s == StateConnected) {
		if (!writeQueue.isEmpty()) {
			if (transmit(writeQueue.head())) {
				writeQueue.dequeue();
			}
		}

		if (writeQueue.isEmpty()) {
			writeNotifier->setEnabled(false);
		}
	}
}
