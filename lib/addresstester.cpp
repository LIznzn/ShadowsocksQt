#include "addresstester.h"
#include "encryptorprivate.h"
#include "encryptor.h"
#include "common.h"

using namespace QSS;

AddressTester::AddressTester(const QHostAddress &_address,
                             const quint16 &_port,
                             QObject *parent) :
    QObject(parent),
    address(_address),
    port(_port),
    testingConnectivity(false)
{
    timer.setSingleShot(true);
    time = QTime::currentTime();
    socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    connect(&timer, &QTimer::timeout, this, &AddressTester::onTimeout);
    connect(&socket, &QTcpSocket::connected, this, &AddressTester::onConnected);
    connect(&socket, &QTcpSocket::readyRead,
            this, &AddressTester::onSocketReadyRead);
    connect(&socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
            (&QTcpSocket::error),
            this,
            &AddressTester::onSocketError);
}

void AddressTester::connectToServer(int timeout)
{
    time = QTime::currentTime();
    timer.start(timeout);
    socket.connectToHost(address, port);
}

void AddressTester::startLagTest(int timeout)
{
    testingConnectivity = false;
    connectToServer(timeout);
}

void AddressTester::startConnectivityTest(const QString &method,
                                          const QString &password,
                                          int timeout)
{
    testingConnectivity = true;
    encryptionMethod = method;
    encryptionPassword = password;
    connectToServer(timeout);
}

void AddressTester::onTimeout()
{
    socket.abort();
    emit connectivityTestFinished(false);
    emit lagTestFinished(LAG_TIMEOUT);
}

void AddressTester::onSocketError(QAbstractSocket::SocketError)
{
    timer.stop();
    socket.abort();
    emit connectivityTestFinished(false);
    emit testErrorString(socket.errorString());
    emit lagTestFinished(LAG_ERROR);
}

void AddressTester::onConnected()
{
    timer.stop();
    emit lagTestFinished(time.msecsTo(QTime::currentTime()));
    if (testingConnectivity) {
        EncryptorPrivate ep(encryptionMethod, encryptionPassword);
        Encryptor encryptor(ep);
        /*
         * A http request to Google to test connectivity
         * The payload is dumped from
         * `curl http://www.google.com --socks5 127.0.0.1:1080`
         *
         * TODO: find a better way to check connectivity
         */
        QByteArray dest =
                Common::packAddress(Address("www.google.com", 80));
        QByteArray payload =
                QByteArray::fromHex("474554202f20485454502f312e310d0a486f73743a"
                                    "207777772e676f6f676c652e636f6d0d0a55736572"
                                    "2d4167656e743a206375726c2f372e34332e300d0a"
                                    "4163636570743a202a2f2a0d0a0d0a");
        socket.write(encryptor.encrypt(dest + payload));
    } else {
        socket.abort();
    }
}

void AddressTester::onSocketReadyRead()
{
    emit connectivityTestFinished(true);
    socket.abort();
}