#ifndef CHACHA_H
#define CHACHA_H

#include <QObject>
#include <QVector>

namespace QSS {

class ChaCha : public QObject {
    Q_OBJECT
public:
    /*
     * Key length must be 32 (16 is dropped)
     * IV length must be 8 or 12
     */
    explicit ChaCha(const QByteArray &_key,
                    const QByteArray &_iv,
                    QObject *parent = 0);

public slots:
    QByteArray update(const QByteArray &input);

private:
    QVector<quint32> m_state;
    QVector<unsigned char> m_buffer;
    quint32 m_position;

    void chacha();
    void setIV(const QByteArray &_iv);
};

}
#endif