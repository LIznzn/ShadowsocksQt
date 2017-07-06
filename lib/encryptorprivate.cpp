#include "encryptorprivate.h"
#include "cipher.h"

using namespace QSS;

EncryptorPrivate::EncryptorPrivate(const QString &m,
                                   const QString &pwd,
                                   QObject *parent) :
        QObject (parent) {
    method = m.toLower().toLocal8Bit();
    password = pwd.toLocal8Bit();
    valid = true;

    Cipher::CipherKeyIVLength ki = Cipher::keyIvMap.at(method);
    method = Cipher::cipherNameMap.at(method);
    if (ki[0] == 0 || !Cipher::isSupported(method)) {
        qCritical("The method %s is not supported.", m.toStdString().data());
        valid = false;
    }
    keyLen = ki[0];
    ivLen = ki[1];
    evpBytesToKey();
}

EncryptorPrivate::EncryptorPrivate(QObject *parent) :
    QObject(parent),
    keyLen(0),
    ivLen(0),
    valid(false)
{}

bool EncryptorPrivate::isValid() const {
    return valid;
}

QString EncryptorPrivate::getInternalMethodName() const {
    return QString(method);
}

void EncryptorPrivate::evpBytesToKey() {
    QVector<QByteArray> m;
    QByteArray data;
    int i = 0;

    while (m.size() < keyLen + ivLen) {
        if (i == 0) {
            data = password;
        } else {
            data = m[i - 1] + password;
        }
        m.append(Cipher::md5Hash(data));
        i++;
    }
    QByteArray ms;
    for (QVector<QByteArray>::ConstIterator it = m.begin();
         it != m.end();
         ++it) {
        ms.append(*it);
    }

    key = ms.mid(0, keyLen);
}

EncryptorPrivate &EncryptorPrivate::operator=(const EncryptorPrivate &o) {
    keyLen = o.keyLen;
    ivLen = o.ivLen;
    method = o.method;
    password = o.password;
    key = o.key;
    valid = o.valid;
    return *this;
}