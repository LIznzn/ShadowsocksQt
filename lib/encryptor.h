#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QObject>
#include "cipher.h"
#include "export.h"
#include "encryptorprivate.h"

namespace QSS {

class QSS_EXPORT Encryptor : public QObject
{
    Q_OBJECT
public:
    explicit Encryptor(const EncryptorPrivate &ep, QObject *parent = 0);

    QByteArray decrypt(const QByteArray &);
    QByteArray encrypt(const QByteArray &);
    QByteArray decryptAll(const QByteArray &);//(de)encryptAll is for updreplay
    QByteArray encryptAll(const QByteArray &);
    void reset();
    QByteArray deCipherIV() const;

    void addHeaderAuth(QByteArray &headerData) const;
    void addHeaderAuth(QByteArray &data, const int &headerLen) const;
    void addChunkAuth(QByteArray &data);

    bool verifyHeaderAuth(const QByteArray &data, const int &headerLen) const;

    /*
     * data will be overwritten by extracted data which can be sent to
     * downstream
     * @return the hash verification result
     */
    bool verifyExtractChunkAuth(QByteArray &data);

private:
    const EncryptorPrivate &ep;
    QByteArray enCipherIV;
    //incomplete data chunk from verifyExtractChunkAuth function
    QByteArray incompleteChunk;
    quint32 chunkId;

protected:
    Cipher *enCipher;
    Cipher *deCipher;
};

}
#endif