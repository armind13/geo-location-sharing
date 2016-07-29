#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include "TcpClient.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> application(SailfishApp::application(argc, argv));
        QScopedPointer<QQuickView> view(SailfishApp::createView());

        QScopedPointer<TcpClient> tcpClient(new TcpClient(view.data()));
        view->rootContext()->setContextProperty("tcpClient", tcpClient.data());

        view->setSource(SailfishApp::pathTo("qml/GeoShare.qml"));
        view->show();

        return application->exec();
}

