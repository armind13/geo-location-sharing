#ifndef GEOLOCATIONSERVER_H
#define GEOLOCATIONSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHash>
#include <QPointF>

const quint16 tcpPortNumber = 43512;

class GeoLocationServer : public QObject
{
    Q_OBJECT
public:
    explicit GeoLocationServer(QObject *parent = 0);
    void init();
    ~GeoLocationServer();

signals:

public slots:

private:
    QTcpServer tcpServer;
    // clients IP and tokens pairs
    QHash<QTcpSocket*, QString> clients;
    QHash<QString, QString> tokensWithInviteKeys;
    QHash<QString, QPointF> tokensWithCoords;

    void createNewClient(QTcpSocket* clientSocket);
    static QString generateUniqueKey();
    void sendConnectAnswer(const QString& token) const;
    QString getServerAddress() const;
    void createInviteKey(const QJsonObject& jsonObj);
    void joinByInviteKey(const QJsonObject& jsonObj);
    void setPosition(const QJsonObject& jsonObj);
    void sendNewPositionToClients(const QJsonObject& jsonObj);

private slots:
    void onNewConnection();
    void readClientData();
    void deleteClients();
};

#endif // GEOLOCATIONSERVER_H
