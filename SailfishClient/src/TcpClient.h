#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QString>
#include <QScopedPointer>

class QTcpSocket;

/** @brief Реализация tcp-клиента */
class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = 0);
    virtual ~TcpClient();

signals:
    /** @brief изменилось состояние подключения */
    void connectionStateChanged();
    /** @brief обновлены данные о геоположении участников */
    void locationUpdated(QString userToken, double latitude, double longitude);
    /** @brief обнолвён ключ приглашения */
    void inviteKeyUpdated(QString key);

public slots:
    void connect(QString hostAddress);
    void disconnect();
    /** @brief Обновить "наше" местоположение */
    void updateLocation(double latitude, double longitude);
    bool isConnected() const;
    /** @brief Получить ключ,
     * при получении которого отправит сигнал inviteKeyUpdated */
    void getInviteKey();
    /** @brief Присоединиться по ключу приложения */
    void joinByInviteKey(QString key);

protected slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    QTcpSocket* socket;
    bool _isConnected;
    QString inviteKey;
    QString token;
};

#endif // TCPCLIENT_H
