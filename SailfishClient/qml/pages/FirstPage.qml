import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        // todo: replace TcpClient to interface
//        TcpClient {
//            id: client
//        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked:
                {
//                    console.log(client)
                    pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
                }

            }
        }

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("GeoShare")
            }
            Label {
                x: Theme.paddingLarge
                text: qsTr("Hello Sailors, we share you now!")
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeMedium
            }
        }
    }
}


