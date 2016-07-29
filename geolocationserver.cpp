#include "GeoLocationServer.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <QDebug>
#include <QNetworkInterface>

GeoLocationServer::GeoLocationServer(QObject *parent) :
    QObject(parent)
{
}

void GeoLocationServer::init()
{
    tcpServer.listen(QHostAddress::AnyIPv4, tcpPortNumber);
    if (tcpServer.isListening())
    {
        qDebug() << "Server address:" << getServerAddress();
        qDebug() << "Port:" << tcpServer.serverPort();
        qDebug() << "Socket descriptor" << tcpServer.socketDescriptor();
    }
    else
    {
        qDebug() << "Server did not started";
    }
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

GeoLocationServer::~GeoLocationServer()
{
    qDebug() << "~GeoLocationServer()";
}

void GeoLocationServer::createNewClient(QTcpSocket* clientSocket)
{
    if (clients.contains(clientSocket))
    {
        return;
    }

    auto token = generateToken();
    clients.insert(clientSocket, token);
    sendConnectAnswer(token);
}

QString GeoLocationServer::generateToken()
{
    return QUuid::createUuid().toString();
}

void GeoLocationServer::sendConnectAnswer(const QString& token) const
{
    auto socket = clients.key(token, nullptr);
    if (nullptr != socket)
    {
        QJsonObject answer;
        answer["type"] = QString("connect_answer");
        answer["token"] = token;
        QJsonDocument document(answer);
        socket->write(document.toBinaryData());
        qDebug() << "Connection package is sent";
        qDebug() << "IP" << socket->localAddress().toString() << "token" << token;
    }
}

QString GeoLocationServer::getServerAddress() const
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for(int i = 0; i < ipAddressesList.count(); ++i)
    {
        if(!ipAddressesList[i].isLoopback() &&
              ipAddressesList[i].protocol() == QAbstractSocket::IPv4Protocol &&
              ipAddressesList[i].toString().startsWith("10."))
        {
            ipAddress = ipAddressesList[i].toString();

        }
    }

    if (ipAddress.isEmpty())
    {
       ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    return ipAddress;
}

void GeoLocationServer::onNewConnection()
{
    while (tcpServer.hasPendingConnections())
    {
        QTcpSocket* clientSocket = tcpServer.nextPendingConnection();
        qDebug() << "New client:" << clientSocket->localAddress().toString();

        connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readClientData()));
        connect(clientSocket, SIGNAL(disconnected()), this, SLOT(deleteClients()));
    }
}

void GeoLocationServer::readClientData()
{
    QTcpSocket* clientSocket = static_cast<QTcpSocket*>(sender());

    auto clientData = clientSocket->readAll();
    auto jsonDocument = QJsonDocument::fromBinaryData(clientData);
    auto jsonObject = jsonDocument.object();

    auto queryType = jsonObject["type"].toString();

    if ("connect" == queryType)
    {
        createNewClient(clientSocket);
        return;
    }
}

void GeoLocationServer::deleteClients()
{

}
