#ifndef CIPHER_H
#define CIPHER_H

#include <array>
#include <map>
#include <QObject>
#include <botan/pipe.h>
#include <botan/version.h>
#include "chacha.h"
#include "export.h"

namespace QSS {

#if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1,10,0)
#error "Botan library is too old."
#elif BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(2,0,0)
typedef Botan::SecureVector<Botan::byte> SecureByteArray;
#define DataOfSecureByteArray(sba) sba.begin()
#else
typedef Botan::secure_vector<Botan::byte> SecureByteArray;
#define DataOfSecureByteArray(sba) sba.data()
#endif

class QSS_EXPORT Cipher : public QObject
{
    Q_OBJECT
public:
    explicit Cipher(const QByteArray &method, const QByteArray &key, const QByteArray &iv, bool encode, QObject *parent = 0);
    Cipher(Cipher &&) = default;
    ~Cipher();

    QByteArray update(const QByteArray &data);
    const QByteArray &getIV() const;

    typedef std::array<int, 2> CipherKeyIVLength;

    /*
     * keyIvMap contains required key length and IV length
     * The CipherKeyIVLength contains two integers, key length and IV length
     * The key of this map is the encryption method (shadowsocks convention)
     */
    static const std::map<QByteArray, CipherKeyIVLength> keyIvMap;

    /*
     * This map stores the shadowsocks convention name and the corresponding
     * cipher name in Botan library
     */
    static const std::map<QByteArray, QByteArray> cipherNameMap;
    static const int AUTH_LEN;

    static QByteArray randomIv(int length);
    static QByteArray hmacSha1(const QByteArray &key, const QByteArray &msg);
    static QByteArray md5Hash(const QByteArray &in);
    static bool isSupported(const QByteArray &method);

    static QList<QByteArray> getSupportedMethodList();

private:
    Botan::Pipe *pipe;
    ChaCha *chacha;
    QByteArray iv;
};

}
#endif