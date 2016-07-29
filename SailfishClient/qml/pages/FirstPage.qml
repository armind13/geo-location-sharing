import QtQuick 2.0
import Sailfish.Silica 1.0
import QtPositioning 5.2
import QtLocation 5.0

Page {
    id: page

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked:
                {
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

            PositionSource {
                id: positionSource
                active: true
                nmeaSource: "/usr/share/device-api-starting-point/nmeafiles/gps.log"
            }
            Label {
                text: "Debug only:"
                x: Theme.horizontalPageMargin
                color: "yellow"
            }
            Column {
                x: Theme.horizontalPageMargin * 1.5
                Label {
                    font.pixelSize: Theme.fontSizeSmall
                    text: "GPS работает: " +
                          positionSource.active
                }
                Label {
                    font.pixelSize: Theme.fontSizeSmall
                    text: "Ширина: " + positionSource.position.
                    coordinate.latitude
                }
                Label {
                    font.pixelSize: Theme.fontSizeSmall
                    text: "Долгота: " + positionSource.position.
                    coordinate.longitude
                }
                Label {
                    id: responsesDebugField
                }
            }

            Connections {
                target: tcpClient
                onLocationUpdated: {
                    console.log("onLocationUpdated")
                    inviteField.text = "Принято: "
                            + "longitude = " + longitude
                            + "latitude = " + latitude
                            + "by token = " + token
                }
            }
        }
    }
}


