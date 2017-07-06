#include <QHostInfo>
#include <QTcpSocket>
#include <botan/init.h>
#include "controller.h"
#include "encryptor.h"

#define FD_SETSIZE 1024

using namespace QSS;

Controller::Controller(bool is_local, bool auto_ban, QObject *parent) :
        QObject(parent),
        valid(true),
        isLocal(is_local),
        autoBan(auto_ban) {
    try {
        Botan::LibraryInitializer::initialize("thread_safe");
    } catch (std::exception &e) {
        qCritical("%s", e.what());
    }

    tcpServer = new TcpServer(ep, profile.timeout,
                              isLocal,
                              autoBan,
                              serverAddress,
                              this);

    //FD_SETSIZE which is the maximum value on *nix platforms. (1024 by default)
    tcpServer->setMaxPendingConnections(FD_SETSIZE);
    udpRelay = new UdpRelay(ep,
                            isLocal,
                            autoBan,
                            serverAddress,
                            this);

    connect(tcpServer, &TcpServer::acceptError,
            this, &Controller::onTcpServerError);
    connect(tcpServer, &TcpServer::info, this, &Controller::info);
    connect(tcpServer, &TcpServer::debug, this, &Controller::debug);
    connect(tcpServer, &TcpServer::bytesRead, this, &Controller::onBytesRead);
    connect(tcpServer, &TcpServer::bytesSend, this, &Controller::onBytesSend);
    connect(tcpServer, &TcpServer::latencyAvailable,
            this, &Controller::tcpLatencyAvailable);

    connect(udpRelay, &UdpRelay::info, this, &Controller::info);
    connect(udpRelay, &UdpRelay::debug, this, &Controller::debug);
    connect(udpRelay, &UdpRelay::bytesRead, this, &Controller::onBytesRead);
    connect(udpRelay, &UdpRelay::bytesSend, this, &Controller::onBytesSend);


    connect(&serverAddress, &Address::lookedUp,
            this, &Controller::onServerAddressLookedUp);
}

Controller::Controller(const Profile &_profile,
                       bool is_local,
                       bool auto_ban,
                       QObject *parent) :
    Controller(is_local, auto_ban, parent)
{
    setup(_profile);
}

Controller::~Controller()
{
    if (tcpServer->isListening()) {
        stop();
    }
    Botan::LibraryInitializer::deinitialize();
}

bool Controller::setup(const Profile &p)
{
    valid = true;
    profile = p;

    /*
     * the default QHostAddress constructor will construct "::" as AnyIPv6
     * we explicitly use Any to enable dual stack
     * which is the case in other shadowsocks ports
     */
    if (p.server == "::") {
        serverAddress = Address(QHostAddress::Any, p.server_port);
    } else {
        serverAddress = Address(p.server, p.server_port);
        serverAddress.lookUp();
    }

    emit info("Initializing encryption algorithm...");
    ep = EncryptorPrivate(profile.method, profile.password);
    if (ep.isValid()) {
        emit info("Encryption: "+ ep.getInternalMethodName() + "(" + profile.method + ")");
    } else {
        emit info("Encryption initialization failed.");
        valid = false;
    }

    if (tcpServer->isListening()) {
        tcpServer->close();
    }
    if (udpRelay->isListening()) {
        udpRelay->close();
    }

    return valid;
}

bool Controller::start()
{
    if (!valid) {
        emit info("Controller is not valid. Maybe improper setup?");
        return false;
    }

    bool listen_ret = false;

    QString sstr("TCP server listen at port ");
    if (isLocal) {
        emit info("Client Mode");
        sstr.append(QString::number(profile.local_port));
        listen_ret = tcpServer->listen(
                    getLocalAddr(),
                    profile.local_port);
        if (listen_ret) {
            listen_ret = udpRelay->listen(getLocalAddr(), profile.local_port);
        }
    } else {
        emit info("Server mode");
        sstr.append(QString::number(profile.server_port));
        listen_ret = tcpServer->listen(serverAddress.getFirstIP(),
                                       profile.server_port);
        if (listen_ret) {
            listen_ret = udpRelay->listen(serverAddress.getFirstIP(),
                                       profile.server_port);
        }
    }

    if (listen_ret) {
        emit info(sstr);
        emit runningStateChanged(true);
    } else {
        emit info("TCP server listen failed.");
    }

    return listen_ret;
}

void Controller::stop()
{
    tcpServer->close();
    udpRelay->close();
    emit runningStateChanged(false);
    emit debug("Stopped.");
}

QHostAddress Controller::getLocalAddr()
{
    QHostAddress addr(profile.local_address);
    if (!addr.isNull()) {
        return addr;
    } else {
        emit info("Can't get address from "
                  + profile.local_address.toLocal8Bit()
                  + ". Using localhost instead.");
        return QHostAddress::LocalHost;
    }
}

void Controller::onTcpServerError(QAbstractSocket::SocketError err)
{
    emit info("TCP server error: " + tcpServer->errorString());

    //can't continue if address is already in use
    if (err == QAbstractSocket::AddressInUseError) {
        stop();
    }
}

void Controller::onBytesRead(const qint64 &r)
{
    if (r != -1) {//-1 means read failed. don't count
        bytesReceived += r;
        emit newBytesReceived(r);
        emit bytesReceivedChanged(bytesReceived);
    }
}

void Controller::onBytesSend(const qint64 &s)
{
    if (s != -1) {//-1 means write failed. don't count
        bytesSent += s;
        emit newBytesSent(s);
        emit bytesSentChanged(bytesSent);
    }
}

void Controller::onServerAddressLookedUp(const bool success, const QString err)
{
    if (!success) {
        emit info("Shadowsocks server DNS lookup failed: " + err);
    }
}