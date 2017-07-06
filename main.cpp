#include <QCoreApplication>
#include <QCommandLineParser>
#include <signal.h>
#include "client.h"

using namespace QSS;

static void onSIGINT_TERM(int sig) {
    if (sig == SIGINT || sig == SIGTERM) qApp->quit();
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    a.setApplicationName("ShadowsocksQt");
    a.setApplicationVersion(Common::version());

    signal(SIGINT, onSIGINT_TERM);
    signal(SIGTERM, onSIGINT_TERM);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription("A fast tunnel proxy that helps you bypass firewalls.");
    QCommandLineOption configFile("c","Proxy configuration file.","config_file","config.json");
    QCommandLineOption serverAddress("s","Domain or IP of your remote server.","server_address");
    QCommandLineOption serverPort("p","Port of your remote server.","server_port");
    QCommandLineOption localAddress("b","IP of your local server. Ignored in server mode.","local_address","127.0.0.1");
    QCommandLineOption localPort("l","Port of your local server. Ignored in server mode.","local_port");
    QCommandLineOption password("k","Password of your remote server.","password");
    QCommandLineOption encryptionMethod("m","Encryption method.","method");
    QCommandLineOption timeout("t","Connection timeout in seconds.","timeout");
    QCommandLineOption serverMode(QStringList() << "S" << "server","Run as Shadowsocks server.");
    QCommandLineOption debug(QStringList() << "D" << "debug","Run in debug mode.");
    QCommandLineOption autoBan(QStringList() << "A" << "autoban","Automatically ban IPs that send malformed header. Ignored in client mode.");

    parser.addOption(configFile);
    parser.addOption(serverAddress);
    parser.addOption(serverPort);
    parser.addOption(localAddress);
    parser.addOption(localPort);
    parser.addOption(password);
    parser.addOption(encryptionMethod);
    parser.addOption(timeout);
    parser.addOption(serverMode);
    parser.addOption(debug);
    parser.addOption(autoBan);
    parser.process(a);

    Client c;

    if (!c.readConfig(parser.value(configFile))) {
        c.setup(parser.value(serverAddress),
                parser.value(serverPort),
                parser.value(localAddress),
                parser.value(localPort),
                parser.value(password),
                parser.value(encryptionMethod),
                parser.value(timeout),
                parser.isSet(debug)
        );
    }
    c.setAutoBan(parser.isSet(autoBan));
    c.setDebug(parser.isSet(debug));

    if (c.start(parser.isSet(serverMode))) {
        QSS::Cipher::getSupportedMethodList();
        return a.exec();
    } else {
        return 2;
    }
}