#include "TcpClient.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

const QString typeKeyword = "type";
const QString getInviteKeyRequestKeyword = "get_invite_key";
const QString getInviteKeyAnswerKeyword ="get_invite_key_answer";
const QString inviteKeyKeyword = "key";
const QString getTokenRequestKeyword ="get_token";
const QString getTokenAnswerKeyword ="get_token_answer";
const QString tokenKeyword = "token";
const QString joinByInviteKeyKeyword = "join_by_invite_key";
const QString newPositionRequestKeyword = "new_position";
const QString newPositionAnswerKeyword = "new_position_answer";
const QString usersKeyword = "users";
const QString latitudeKeyword = "latitude";
const QString longitudeKeyword = "longitude";

TcpClient::TcpClient(QObject *parent)
    : QObject(parent),
      socket(0),
      _isConnected(false)
{
}

TcpClient::~TcpClient()
{
}

void TcpClient::connect(QString hostAddress)
{
    if (socket)
    {
        if (socket->isOpen())
        {
            socket->disconnectFromHost();
        }
        delete socket;
    }

    socket = new QTcpSocket(this);
    _isConnected = false;
    emit connectionStateChanged();
    QObject::connect(socket, &QIODevice::readyRead, this, &TcpClient::onReadyRead);
    QObject::connect(socket, &QAbstractSocket::connected, this, &TcpClient::onConnected);
    QObject::connect(socket, &QAbstractSocket::connected, this, &TcpClient::onDisconnected);
    socket->connectToHost(hostAddress, 43512);
}

void TcpClient::disconnect()
{
    socket->disconnectFromHost();
}

void TcpClient::updateLocation(double latitude, double longitude)
{
    if (!isConnected())
    {
        return;
    }

    QJsonObject request;
    request[typeKeyword] = newPositionRequestKeyword;
    request[tokenKeyword] = token;
    request[inviteKeyKeyword] = inviteKey;
    request[latitudeKeyword] = latitude;
    request[longitudeKeyword] = longitude;
    QJsonDocument document(request);
    socket->write(document.toBinaryData());
    socket->flush();
}

bool TcpClient::isConnected() const
{
    return _isConnected;
}

void TcpClient::getInviteKey()
{
    if (isConnected())
    {
        QJsonObject request;
        request[typeKeyword] = getInviteKeyRequestKeyword;
        request[tokenKeyword] = token;
        QJsonDocument document(request);
        socket->write(document.toBinaryData());
        socket->flush();
    }
}

void TcpClient::joinByInviteKey(QString key)
{
    if (!isConnected())
    {
        return;
    }
    inviteKey = key;

    QJsonObject request;
    request[typeKeyword] = joinByInviteKeyKeyword;
    request[tokenKeyword] = token;
    request[inviteKeyKeyword] = inviteKey;
    QJsonDocument document(request);
    socket->write(document.toBinaryData());
    socket->flush();
}

void TcpClient::onReadyRead()
{
    const QByteArray response = socket->readAll();
    const QJsonDocument jsonDocument = QJsonDocument::fromBinaryData(response);
    const QJsonObject jsonObject = jsonDocument.object();
    const QJsonValue typeValue = jsonObject[typeKeyword];
    if (typeValue.isNull() && !typeValue.isString())
    {
        return;
    }

    const QString typeValueString = typeValue.toString();

    // ответ на get_token
    if (typeValueString == getTokenAnswerKeyword)
    {
        const QJsonValue tokenValue = jsonObject[tokenKeyword];
        if (tokenValue.isString() && !tokenValue.isNull())
        {
            _isConnected = true;
            token = tokenValue.toString();
            emit connectionStateChanged();
        }
        return;
    }

    // ответ на get_invite_key
    if (typeValueString == getInviteKeyAnswerKeyword)
    {
        const QJsonValue inviteKeyValue = jsonObject[inviteKeyKeyword];
        if (inviteKeyValue.isString() && !inviteKeyValue.isNull())
        {
            inviteKey = inviteKeyValue.toString();
            emit inviteKeyUpdated(inviteKey);
        }
        return;
    }
}

void TcpClient::onConnected()
{
    // запрашиваем токен от сервера
    QJsonObject request;
    request[typeKeyword] = getTokenRequestKeyword;
    QJsonDocument document(request);
    socket->write(document.toBinaryData());
    socket->flush();
}

void TcpClient::onDisconnected()
{
    _isConnected = false;
    emit connectionStateChanged();
}
