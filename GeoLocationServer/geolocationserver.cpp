#include "GeoLocationServer.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDebug>
#include <QNetworkInterface>

#include "StringConstants.h"

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
    qDebug() << "---";
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

    auto token = generateUniqueKey();
    clients.insert(clientSocket, token);
    sendConnectAnswer(token);
}

QString GeoLocationServer::generateUniqueKey()
{
    return QUuid::createUuid().toString();
}

void GeoLocationServer::sendConnectAnswer(const QString& token) const
{
    auto socket = clients.key(token, nullptr);
    if (nullptr != socket)
    {
        QJsonObject answer;
        answer[TYPE] = GET_TOKEN_ANSWER;
        answer[TOKEN] = token;
        QJsonDocument document(answer);
        socket->write(document.toBinaryData());
        socket->flush();
        qDebug() << "Connection package is sent";
        qDebug() << "IP" << socket->localAddress().toString() << "token" << token;
        qDebug() << "---";
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

void GeoLocationServer::createInviteKey(const QJsonObject& jsonObj)
{
    auto token = jsonObj[TOKEN].toString();
    auto socket = clients.key(token, nullptr);
    if (nullptr != socket)
    {
        auto inviteKey = generateUniqueKey();
        tokensWithInviteKeys.insert(token, inviteKey);

        QJsonObject answer;
        answer[TYPE] = GET_INVITE_KEY_ANSWER;
        answer[KEY] = inviteKey;
        QJsonDocument document(answer);
        socket->write(document.toBinaryData());
        socket->flush();
        qDebug() << "Invite key sent";
        qDebug() << "IP" << socket->localAddress().toString() << "key" << inviteKey;
        qDebug() << "---";
    }
}

void GeoLocationServer::joinByInviteKey(const QJsonObject& jsonObj)
{
    auto key = jsonObj[KEY].toString();
    auto token = jsonObj[TOKEN].toString();
    tokensWithInviteKeys.insert(token, key);

    auto tokens = tokensWithInviteKeys.keys(key);
    QJsonArray usersArray;
    for (auto currentToken : tokens)
    {
        if (currentToken != token && tokensWithCoords.contains(currentToken))
        {
            auto position = tokensWithCoords[currentToken];
            QJsonObject userObj;
            userObj[TOKEN] = currentToken;
            userObj[LATITUDE] = position.x();
            userObj[LONGITUDE] = position.y();
            usersArray.push_back(QJsonValue(userObj));
        }
    }
    QJsonObject answer;
    answer[TYPE] = JOIN_BY_INVITE_KEY_ANSWER;
    answer[USERS] = usersArray;

    auto socket = clients.key(token, nullptr);
    if (nullptr != socket)
    {
        QJsonDocument document(answer);
        socket->write(document.toBinaryData());
        socket->flush();
        qDebug() << TOKEN << token << "joined to group with key" << key;
        qDebug() << "---";
    }
}

void GeoLocationServer::setPosition(const QJsonObject& jsonObj)
{
    auto token = jsonObj[TOKEN].toString();

    QPointF position;
    position.setX(jsonObj[LATITUDE].toDouble());
    position.setY(jsonObj[LONGITUDE].toDouble());
    tokensWithCoords[token] = position;
}

void GeoLocationServer::sendNewPositionToClients(const QJsonObject& jsonObj)
{
    auto srcToken = jsonObj[TOKEN].toString();
    auto key = jsonObj[KEY].toString();

    QJsonObject answer;
    answer[TYPE] = NEW_POSITION_ANSWER;
    answer[KEY] = key;
    answer[TOKEN] = srcToken;
    answer[LATITUDE] = tokensWithCoords.value(srcToken).x();
    answer[LONGITUDE] = tokensWithCoords.value(srcToken).y();
    QJsonDocument document(answer);

    auto destTokens = tokensWithInviteKeys.keys(key);
    for (auto currentToken : destTokens)
    {
        auto socket = clients.key(currentToken, nullptr);
        if (nullptr != socket)
        {
            socket->write(document.toBinaryData());
            socket->flush();
            qDebug() << "Position sent from token" << srcToken << "to" << currentToken << "with key" << key;
            qDebug() << "---";
        }
    }
}

void GeoLocationServer::onNewConnection()
{
    while (tcpServer.hasPendingConnections())
    {
        QTcpSocket* clientSocket = tcpServer.nextPendingConnection();
        qDebug() << "New client:" << clientSocket->localAddress().toString();
        qDebug() << "---";

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

    auto queryType = jsonObject[TYPE].toString();

    if (GET_TOKEN == queryType)
    {
        createNewClient(clientSocket);
        return;
    }
    if (GET_INVITE_KEY == queryType)
    {
        createInviteKey(jsonObject);
        return;
    }
    if (JOIN_BY_INVITE_KEY == queryType)
    {
        joinByInviteKey(jsonObject);
        return;
    }
    if (NEW_POSITION == queryType)
    {
        setPosition(jsonObject);
        sendNewPositionToClients(jsonObject);
        return;
    }
}

void GeoLocationServer::deleteClients()
{
    foreach(auto socket, clients.keys())
    {
        if (nullptr != socket && (socket->socketDescriptor() == -1))
        {
            auto token = clients.value(socket);
            clients.remove(socket);
            tokensWithInviteKeys.remove(token);
            qDebug() << "DISCONNECTED: " << token;
            qDebug() << "---";
        }
    }
}
