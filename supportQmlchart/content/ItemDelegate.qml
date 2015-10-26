import QtQuick 2.3

Item {
    id: itemDelegate

    property string name: "?"
    property string personalId: "0"

    Rectangle {
        id: box
        anchors.fill: parent
        border.color: "black"
        radius: 10
        gradient: Gradient {
            GradientStop{position: 0; color: "gray"}
            GradientStop{position: 1; color: "white"}
        }
    }

    Row {
        id: row

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        height: parent.height
        spacing: 6

        Image {
            id: icon
            source: "http://sai.local/images/grid48x48.gif"
            scale: 0.3
            anchors.verticalCenter: row.verticalCenter
        }

        Text {
            anchors.verticalCenter: row.verticalCenter
            width: 150
            text: name
        }

        Text {
            anchors.verticalCenter: row.verticalCenter
            width: 150
            text: personalId
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        onClicked: {
            console.log("onClicked Deligate" + name);
        }
    }
}

