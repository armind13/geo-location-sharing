import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    anchors.fill: parent

    SilicaFlickable {
        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            TextField {
                label: "host url"
                placeholderText: "localhost"
                text: "localhost"
                id: urlField
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Button {
                id: button_connect
                text: "Connect"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked:
                {
                    console.log("connect clicked")
                    tcpClient.connect(urlField.text)
                }
            }

            Connections {
                target: tcpClient
                onConnectionStateChanged: {
                    console.log("onConnectionStateChanged")
                    if(tcpClient.isConnected()){
                        button_connect.enabled = false
                        button_get_invite_key.enabled = true
                        button_sendTestLocation.enabled = true
                    }
                    else
                    {
                        button_connect.enabled = true
                        button_get_invite_key.enabled = false
                        button_sendTestLocation.enabled = false
                    }
                }
                onInviteKeyUpdated: {
                    console.log("onInviteKeyUpdated")
                    inviteField.text = key
                }
            }

            TextField {
                label: "invite key"
                placeholderText: "Write key to join or push \"Get\""
                id: inviteField
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Button {
                id: button_get_invite_key
                text: "Get key"
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: false
                onClicked:
                {
                    console.log("get invite key clicked")
                    tcpClient.getInviteKey()
                }
            }

            Label {
                text: "Debug only:"
                x: Theme.horizontalPageMargin
                color: "yellow"
            }

            TextField {
                label: "latitude"
                text: "55.45"
                id: latitudeField
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                validator: DoubleValidator {
                    bottom: 0
                    top: 90
                    decimals: 2
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
            TextField {
                label: "longitude"
                text: "48.44"
                id: longitudeField
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                validator: DoubleValidator {
                    bottom: -180
                    top: 180
                    decimals: 2
                }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }

            Button {
                id: button_sendTestLocation
                text: "send test location"
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: false
                onClicked:
                {
                    console.log("send test location clicked")
                    tcpClient.updateLocation(
                                parseFloat(latitudeField.text),
                                parseFloat(longitudeField.text));
                }
            }
        }
        VerticalScrollDecorator {}
    }
}





