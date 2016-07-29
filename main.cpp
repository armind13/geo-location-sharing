#include <QCoreApplication>

#include "GeoLocationServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GeoLocationServer server;
    server.init();
    return a.exec();
}
