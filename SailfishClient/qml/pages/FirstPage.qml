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
                onPositionChanged: {
                    var coord = positionSource.position.coordinate;
                    console.log("Coordinate:", coord.longitude, coord.latitude);
                    tcpClient.updateLocation(coord.latitude, coord.longitude)
                    mePositionMarker.center = QtPositioning.coordinate(coord.latitude, coord.longitude)
                }
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
                    text: "Широта: " + positionSource.position.
                    coordinate.latitude
                }
                Label {
                    font.pixelSize: Theme.fontSizeSmall
                    text: "Долгота: " + positionSource.position.
                    coordinate.longitude
                }
                Label {
                    id: responsesDebugField
                    font.pixelSize: Theme.fontSizeExtraSmall
                }
            }

            Connections {
                target: tcpClient
                onLocationUpdated: {
                    console.log("onLocationUpdated")
                    responsesDebugField.text = "Принято:"
                            + " l = " + latitude
                            + " b = " + longitude
                            + " by token = " + userToken
                    var coords = QtPositioning.coordinate(latitude, longitude)
                    map.center = coords
                    friendPositionMarker.center = coords
                }
            }

            Plugin {
                id: osmPlugin
                allowExperimental: true
                preferred: ["osm"]
                required.mapping: Plugin.AnyMappingFeatures
                required.geocoding: Plugin.AnyGeocodingFeatures
            }
            Rectangle {
                height: applicationWindow.height
                width: applicationWindow.width

                property MapCircle circle

                Map {
                    id: map
                    anchors.fill: parent
                    plugin: osmPlugin
                    zoomLevel: 13
                    gesture.enabled: true
                    center {
                        id: coords
                        latitude: 55.45
                        longitude: 48.44
                    }

                    MapCircle {
                        id: mePositionMarker
                        center {
                            latitude: 55.45
                            longitude: 48.44
                        }
                        radius: 2000.0
                        color: 'red'
                        border.width: 3
                    }

                    MapCircle {
                        id: friendPositionMarker
                        center {
                            latitude: 55.45
                            longitude: 48.44
                        }
                        radius: 2000.0
                        color: 'blue'
                        border.width: 3
                    }
                }
            }
        } // Column
    } // SilicaFlickable
}


