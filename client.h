#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTextStream>
#include "lib/QtSS.h"

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    bool readConfig(const QString &);

    void setup(const QString &remote_addr,
               const QString &remote_port,
               const QString &local_addr,
               const QString &local_port,
               const QString &password,
               const QString &method,
               const QString &timeout,
               const bool debug);

    void setAutoBan(bool ban);
    void setDebug(bool debug);
    QString getMethod() const;

public slots:
    bool start(bool _server = false);

private:
    QSS::Controller *lc;
    QSS::Profile profile;
    bool autoBan;
    bool headerTest();

private slots:
    void logHandler(const QString &);
    void onConnectivityResultArrived(bool);
};

#endif
