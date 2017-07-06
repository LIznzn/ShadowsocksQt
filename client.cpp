#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "client.h"

Client::Client(QObject *parent) :
    QObject(parent),
    lc(nullptr),
    autoBan(false)
{}

bool Client::readConfig(const QString &file) {
    QFile c(file);
    if (!c.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QSS::Common::qOut << "can't open config file " << file << endl;
        return false;
    }
    if (!c.isReadable()) {
        QSS::Common::qOut << "config file " << file
                          << " is not readable!" << endl;
        return false;
    }
    QByteArray confArray = c.readAll();
    c.close();

    QJsonDocument confJson = QJsonDocument::fromJson(confArray);
    QJsonObject confObj = confJson.object();
    profile.local_address = confObj["local_address"].toString();
    profile.local_port = confObj["local_port"].toInt();
    profile.method = confObj["method"].toString();
    profile.password = confObj["password"].toString();
    profile.server = confObj["server"].toString();
    profile.server_port = confObj["server_port"].toInt();
    profile.timeout = confObj["timeout"].toInt();

    return true;
}

void Client::setup(const QString &remote_addr,
                   const QString &remote_port,
                   const QString &local_addr,
                   const QString &local_port,
                   const QString &password,
                   const QString &method,
                   const QString &timeout,
                   const bool debug) {
    profile.server = remote_addr;
    profile.server_port = remote_port.toInt();
    profile.local_address = local_addr;
    profile.local_port = local_port.toInt();
    profile.password = password;
    profile.method = method;
    profile.timeout = timeout.toInt();
    profile.debug = debug;
}

void Client::setAutoBan(bool ban) {
    autoBan = ban;
}

void Client::setDebug(bool debug) {
    profile.debug = debug;
}

bool Client::start(bool _server) {
    if (profile.debug) {
        if (!headerTest()) {
            QSS::Common::qOut << "Header test failed" << endl;
            return false;
        }
    }

    if (lc) {
        lc->deleteLater();
    }
    lc = new QSS::Controller(!_server, autoBan, this);
    connect (lc, &QSS::Controller::info, this, &Client::logHandler);
    if (profile.debug) {
        connect(lc, &QSS::Controller::debug, this, &Client::logHandler);
    }
    lc->setup(profile);

    if (!_server) {
        QSS::Address server(profile.server, profile.server_port);
        QSS::AddressTester *tester = new QSS::AddressTester(server.getFirstIP(), server.getPort());
        connect(tester, &QSS::AddressTester::connectivityTestFinished, this, &Client::onConnectivityResultArrived);
        tester->startConnectivityTest(profile.method, profile.password);
    }

    return lc->start();
}

bool Client::headerTest() {
    int length;
    QHostAddress test_addr("1.2.3.4");
    QHostAddress test_addr_v6("2001:0db8:85a3:0000:0000:8a2e:1010:2020");
    quint16 test_port = 56;
    QSS::Address test_res, test_v6(test_addr_v6, test_port);
    QByteArray packed = QSS::Common::packAddress(test_v6);
    QSS::Common::parseHeader(packed, test_res, length);
    bool success = (test_v6 == test_res);
    if (!success) {
        QSS::Common::qOut << test_v6.toString() << " --> "
                          << test_res.toString() << endl;
    }
    packed = QSS::Common::packAddress(test_addr, test_port);
    QSS::Common::parseHeader(packed, test_res, length);
    bool success2 = ((test_res.getFirstIP() == test_addr)
                 && (test_res.getPort() == test_port));
    if (!success2) {
        QSS::Common::qOut << test_addr.toString().toLocal8Bit()
                          << ":" << test_port << " --> "
                          << test_res.toString() << endl;
    }
    return success & success2;
}

void Client::logHandler(const QString &log) {
    QSS::Common::qOut << log << endl;
}

QString Client::getMethod() const {
    return profile.method;
}

void Client::onConnectivityResultArrived(bool c) {
    if (c) {
        QSS::Common::qOut << "Proxy connection is online." << endl;
    } else {
        QSS::Common::qOut << "Destination is not reachable. "
                             "Please check your network and firewall. "
                             "Make sure the profile is correct."
                          << endl;
    }
    sender()->deleteLater();
}