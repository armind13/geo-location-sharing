#ifndef IGEOSHARECLIENT
#define IGEOSHARECLIENT

#include <QUrl>
#include <QObject>
#include <QDebug>

/** @brief Интерфейс клиента */
class GeoShareClientBase : public QObject
{
    Q_OBJECT

public:
    explicit GeoShareClientBase(QObject *parent = 0)
        : QObject(parent){}
    virtual ~GeoShareClientBase(){}

public slots:
    /** @brief Подключиться */
    virtual void connect(QUrl){}
    virtual void disconnect()
    {
        qDebug() << "GeoShareClientBase::disconnect";
    }

    /** @brief Отправить свою позицию */
    virtual void updateLocation(/* some <Position> structure */){}

 signals:
    /** @brief Подключено
     * @return true - успешно, иначе - нет
    */
    bool connected();

    // todo: return type must be QPair<ClientId, Position>
    void locationUpdated();
};

#endif // IGEOSHARECLIENT

