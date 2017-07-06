#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QHostAddress>
#include <QVector>
#include <QMutex>
#include "address.h"

namespace QSS {

//fist: r; second: d
typedef QPair<Address, Address> CacheKey;

namespace Common {

QSS_EXPORT const QByteArray version();
QSS_EXPORT QByteArray packAddress(const Address &addr);
//this will never use ADDRTYPE_HOST because addr is an IP address
QSS_EXPORT QByteArray packAddress(const QHostAddress &addr,
                                  const quint16 &port);
QSS_EXPORT void parseHeader(const QByteArray &data,
                            Address &addr,
                            int &length);
QSS_EXPORT void exclusive_or(unsigned char *ks,
                             const unsigned char *in,
                             unsigned char *out,
                             quint32 length);
QSS_EXPORT void banAddress(const QHostAddress &addr);
QSS_EXPORT bool isAddressBanned(const QHostAddress &addr);

extern QTextStream qOut;
extern QVector<QHostAddress> bannedAddressVector;
extern QMutex bannedAddressMutex;

extern const quint8 ADDRESS_MASK;
}

}
#endif
