#ifndef ENCRYPTORPRIVATE
#define ENCRYPTORPRIVATE

#include <QByteArray>
#include <QVector>
#include <QObject>
#include "export.h"

namespace QSS {

class QSS_EXPORT EncryptorPrivate : public QObject {
    Q_OBJECT
public:
    /*
     * Initialise an EncryptorPrivate instance that is needed by Encryptor
     * Multiple encryptors can share one EncryptorPrivate so long as they're
     * using same encryption method and password.
     * If the initialisation doesn't succeed, isValid() function return false
     */
    explicit EncryptorPrivate(const QString &method,
                              const QString &password,
                              QObject *parent = 0);

    //construct an invalid/null instance
    explicit EncryptorPrivate(QObject *parent = 0);

    bool isValid() const;

    /*
     * Because we use a different name internally,
     * i.e. aes-128-cfb becomes AES-128/CFB. This function may be helpful
     * for developers to diagnose the problem (if there is one).
     */
    QString getInternalMethodName() const;

    EncryptorPrivate& operator= (const EncryptorPrivate &o);

private:
    int keyLen;
    int ivLen;
    QByteArray method;
    QByteArray password;
    QByteArray key;
    bool valid;

    void evpBytesToKey();

    friend class Encryptor;
};

}
#endif

