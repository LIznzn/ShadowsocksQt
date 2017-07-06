#ifndef UDPRELAY_H
#define UDPRELAY_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMap>
#include "address.h"
#include "encryptor.h"

namespace QSS {

class QSS_EXPORT UdpRelay : public QObject {
    Q_OBJECT
public:
    explicit UdpRelay(const EncryptorPrivate &ep,
                      const bool &is_local,
                      const bool &auto_ban,
                      const Address &serverAddress,
                      QObject *parent = 0);

    void setup(const QHostAddress &localAddr,
               const quint16 &localPort);

    bool isListening() const;

public slots:
    bool listen(const QHostAddress& addr, quint16 port);
    void close();

signals:
    void debug(const QString &);
    void info(const QString &);

    /*
     * The same situation here.
     * We only count "listen" socket's read and written bytes
     */
    void bytesRead(const qint64 &);
    void bytesSend(const qint64 &);

private:
    //64KB, same as shadowsocks-python (udprelay)
    static const qint64 RemoteRecvSize = 65536;

    const Address &serverAddress;
    const bool &isLocal;
    const bool &autoBan;
    QUdpSocket listenSocket;
    Encryptor *encryptor;

    QMap<Address, QUdpSocket*> cache;

private slots:
    void onSocketError();
    void onListenStateChanged(QAbstractSocket::SocketState);
    void onServerUdpSocketReadyRead();
    void onClientUdpSocketReadyRead();
    void onClientDisconnected();
};

}
#endif
