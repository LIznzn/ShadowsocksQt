#include "common.h"
#include "address.h"

using namespace QSS;

Address::Address(const QString &a, const quint16 &p, QObject *parent):QObject(parent) {
    data.second = p;
    setAddress(a);
}

Address::Address(const QHostAddress &ip, const quint16 &p, QObject *parent):QObject(parent) {
    data.second = p;
    setIPAddress(ip);
}

Address::Address(const Address &o):QObject(o.parent()) {
    *this = o;
}

QString Address::getAddress() const {
    return data.first;
}

QHostAddress Address::getFirstIP() const {
    if(ipAddrList.isEmpty()){
        return QHostAddress();
    } else {
        return ipAddrList.first();
    }
}

bool Address::isIPValid() const {
    return !ipAddrList.isEmpty();
}

quint16 Address::getPort() const {
    return data.second;
}

void Address::lookUp() {
    if (isIPValid()) {
        emit lookedUp(true, QString());
    } else {
        QHostInfo::lookupHost(data.first,this,SLOT(onLookUpFinished(QHostInfo)));
    }
}

void Address::blockingLookUp() {
    if (isIPValid()) {
        return;
    }
    QHostInfo result = QHostInfo::fromName(data.first);
    ipAddrList = result.addresses();
}

void Address::setAddress(const QString &a) {
    data.first = a.trimmed();
    ipAddrList.clear();
    QHostAddress ipAddress(a);
    if (!ipAddress.isNull()) {
        ipAddrList.append(ipAddress);
    }
}

void Address::setIPAddress(const QHostAddress &ip) {
    ipAddrList.clear();
    ipAddrList.append(ip);
    data.first = ip.toString();
}

void Address::setPort(const quint16 &p) {
    data.second = p;
}

Address::ATYP Address::addressType() const {
    QHostAddress ipAddress(data.first);
    if (ipAddress.isNull()) {//cannot be parsed -> domain
        return HOST;
    } else if (ipAddress.protocol() == QAbstractSocket::IPv4Protocol) {
        return IPV4;
    } else {
        return IPV6;
    }
}

QString Address::toString() const {
    return QString("%1:%2").arg(data.first).arg(QString::number(data.second));
}

Address &Address::operator= (const Address &o) {
    this->data = o.data;
    this->ipAddrList = o.ipAddrList;
    return *this;
}

void Address::onLookUpFinished(const QHostInfo &host) {
    if (host.error() != QHostInfo::NoError) {
        emit lookedUp(false, host.errorString());
    } else {
        ipAddrList = host.addresses();
        emit lookedUp(true, QString());
    }
}