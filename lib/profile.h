#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QByteArray>

namespace QSS {

struct Profile {
    QString nameTag;//Profile "name"
    QString server;
    QString local_address;
    QString method;
    QString password;
    quint16 server_port;
    quint16 local_port;
    int timeout;
    bool debug;

    Profile();
};

}
#endif
