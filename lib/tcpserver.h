#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include "address.h"
#include "encryptorprivate.h"
#include "export.h"
#include "tcprelay.h"

namespace QSS {

class QSS_EXPORT TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(const EncryptorPrivate &ep,
                       const int &timeout,
                       const bool &is_local,
                       const bool &auto_ban,
                       const Address &serverAddress,
                       QObject *parent = nullptr);
    ~TcpServer();

    bool listen(const QHostAddress &address, quint16 port);
    void close();

signals:
    void debug(const QString &);
    void info(const QString &);
    void bytesRead(const qint64 &);
    void bytesSend(const qint64 &);
    void latencyAvailable(const int &);

protected:
    void incomingConnection(qintptr handler) Q_DECL_OVERRIDE;

private:
    const bool &isLocal;
    const bool &autoBan;
    const Address &serverAddress;
    const int &timeout;
    const EncryptorPrivate &ep;

    QList<TcpRelay*> conList;
    QList<QThread*> threadList;
    quint64 workerThreadID;
    quint64 totalWorkers;

private slots:
    void onConnectionFinished();
};

}
#endif