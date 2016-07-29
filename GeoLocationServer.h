#ifndef GEOLOCATIONSERVER_H
#define GEOLOCATIONSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHash>

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

    void createNewClient(QTcpSocket* clientSocket);
    static QString generateToken();
    void sendConnectAnswer(const QString& token) const;
    QString getServerAddress() const;

private slots:
    void onNewConnection();
    void readClientData();
    void deleteClients();
};

#endif // GEOLOCATIONSERVER_H
