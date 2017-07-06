#include <QTextStream>
#include <QHostInfo>
#include <QtEndian>
#include <random>
#include "common.h"

#define QSS_VERSION "0.0.1"

using namespace QSS;

QTextStream Common::qOut(stdout, QIODevice::WriteOnly | QIODevice::Unbuffered);
QVector<QHostAddress> Common::bannedAddressVector;
QMutex Common::bannedAddressMutex;
const quint8 Common::ADDRESS_MASK = 0b00001111;//0xf

const QByteArray Common::version()
{
    return QSS_VERSION;
}

//pack a shadowsocks header
QByteArray Common::packAddress(const Address &addr)
{
    QByteArray addr_bin, port_ns;
    port_ns.resize(2);
    qToBigEndian(addr.getPort(), reinterpret_cast<uchar*>(port_ns.data()));

    Address::ATYP type = addr.addressType();
    if (type == Address::HOST) {
        QByteArray address_str = addr.getAddress().toLocal8Bit();
        //can't be longer than 255
        addr_bin.append(static_cast<char>(address_str.length()));
        addr_bin += address_str;
    } else if (type == Address::IPV4) {
        quint32 ipv4_addr = qToBigEndian(addr.getFirstIP().toIPv4Address());
        addr_bin = QByteArray(reinterpret_cast<char*>(&ipv4_addr), 4);
    } else {
        //Q_IPV6ADDR is a 16-unsigned-char struct (big endian)
        Q_IPV6ADDR ipv6_addr = addr.getFirstIP().toIPv6Address();
        addr_bin = QByteArray(reinterpret_cast<char*>(ipv6_addr.c), 16);
    }

    char type_c = static_cast<char>(type);

    return type_c + addr_bin + port_ns;
}

QByteArray Common::packAddress(const QHostAddress &addr,
                               const quint16 &port)
{
    QByteArray addr_bin, port_ns;
    char type_c;
    port_ns.resize(2);
    qToBigEndian(port, reinterpret_cast<uchar*>(port_ns.data()));
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4_addr = qToBigEndian(addr.toIPv4Address());
        type_c = static_cast<char>(Address::IPV4);
        addr_bin = QByteArray(reinterpret_cast<char*>(&ipv4_addr), 4);
    } else {
        type_c = static_cast<char>(Address::IPV6);
        Q_IPV6ADDR ipv6_addr = addr.toIPv6Address();
        addr_bin = QByteArray(reinterpret_cast<char*>(ipv6_addr.c), 16);
    }
    return type_c + addr_bin + port_ns;
}

void Common::parseHeader(const QByteArray &data,
                         Address &dest,
                         int &header_length)
{
    char atyp = data[0];
    int addrtype = static_cast<int>(atyp & ADDRESS_MASK);
    header_length = 0;

    if (addrtype == Address::HOST) {
        if (data.length() > 2) {
            quint8 addrlen = static_cast<quint8>(data[1]);
            if (data.size() >= 2 + addrlen) {
                dest.setPort(qFromBigEndian(*reinterpret_cast<const quint16 *>
                                            (data.data() + 2 + addrlen))
                             );
                dest.setAddress(QString(data.mid(2, addrlen)));
                header_length = 4 + addrlen;
            }
        }
    } else if (addrtype == Address::IPV4) {
        if (data.length() >= 7) {
            QHostAddress addr(qFromBigEndian(*reinterpret_cast<const quint32 *>
                                             (data.data() + 1))
                              );
            if (!addr.isNull()) {
                header_length = 7;
                dest.setIPAddress(addr);
                dest.setPort(qFromBigEndian(*reinterpret_cast<const quint16 *>
                                            (data.data() + 5))
                             );
            }
        }
    } else if (addrtype == Address::IPV6) {
        if (data.length() >= 19) {
            Q_IPV6ADDR ipv6_addr;
            memcpy(ipv6_addr.c, data.data() + 1, 16);
            QHostAddress addr(ipv6_addr);
            if (!addr.isNull()) {
                header_length = 19;
                dest.setIPAddress(addr);
                dest.setPort(qFromBigEndian(*reinterpret_cast<const quint16 *>
                                            (data.data() + 17))
                             );
            }
        }
    }
}

void Common::exclusive_or(unsigned char *ks,
                          const unsigned char *in,
                          unsigned char *out,
                          quint32 length)
{
    unsigned char *end_ks = ks + length;
    do {
        *out = *in ^ *ks;
        ++out; ++in; ++ks;
    } while (ks < end_ks);
}

void Common::banAddress(const QHostAddress &addr)
{
    bannedAddressMutex.lock();
    bannedAddressVector.append(addr);
    bannedAddressMutex.unlock();
}

bool Common::isAddressBanned(const QHostAddress &addr)
{
    bannedAddressMutex.lock();
    bool banned = bannedAddressVector.contains(addr);
    bannedAddressMutex.unlock();
    return banned;
}
