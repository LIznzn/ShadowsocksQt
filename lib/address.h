#ifndef ADDRESS_H
#define ADDRESS_H

#include <QString>
#include <QHostAddress>
#include <QHostInfo>
#include <QPair>
#include <QObject>
#include "export.h"

namespace QSS {

class QSS_EXPORT Address : public QObject {
    Q_OBJECT
public:

    explicit Address(const QString &a = QString(), const quint16 &p = 0, QObject *parent = 0);
    explicit Address(const QHostAddress &ip, const quint16 &p, QObject *parent = 0);

    Address(const Address &o);
    Address(Address &&) = default;

    QString getAddress() const;

    /*
     * detect IPv4/IPv6 reachability automatically
     * Normally the order is platform-dependent and it'd consider IPv4 and IPv6
     * precedence, which *might* be more suitable to use this function to get
     * a reachable IP address
     * If there is no valid IP, a default constructed QHostAddress is returned.
     */
    QHostAddress getFirstIP() const;

    bool isIPValid() const;
    quint16 getPort() const;

    /*
     * lookedUp signal will pass if it's successful
     * then you can use getRandomIP() to get a random IP address
     * Note this function will emit lookedUp signal immediately if there is
     * already a valid IP
     */
    void lookUp();
    void blockingLookUp();

    void setAddress(const QString &);
    void setIPAddress(const QHostAddress &);
    void setPort(const quint16 &);

    enum ATYP { IPV4 = 1, IPV6 = 4, HOST = 3 };

    ATYP addressType() const;

    QString toString() const;

    Address& operator= (const Address &o);

    inline bool operator< (const Address &o) const {
        return this->data < o.data;
    }

    inline bool operator== (const Address &o) const {
        return this->data == o.data;
    }

    friend inline QDataStream& operator<< (QDataStream &os,
                                           const Address &addr) {
        return os << addr.toString();
    }

    friend inline QDebug& operator<< (QDebug &os, const Address &addr) {
        return os << addr.toString();
    }

signals:
    void lookedUp(const bool success, const QString errStr);

private:
    QPair<QString, quint16> data;//first: address string; second: port
    QList<QHostAddress> ipAddrList;

private slots:
    void onLookUpFinished(const QHostInfo &host);
};

}
#endif