import QtQuick 2.3

Item {
    id: countCallsDelegate

    property string name: "name"
    property string personalId: "Id"

    property color gradientColorBegin: "white"
    property color gradientColorEnd: "gray"

    property var view: ListView.view

    signal selected()

    function updateSelectedRow()
    {
        if (view.previousItem) {
            view.previousItem.setGradientColorUnselected();
        }
        countCallsDelegate.selected();
        setGradientColorSelected();
        view.currentIndex = model.index;
    }

    function setGradientColorSelected() {
        gradientColorBegin = "white";
        gradientColorEnd = "blue";
    }

    function setGradientColorUnselected() {
        gradientColorBegin = "white";
        gradientColorEnd = "gray";
    }

    Rectangle {
        id: box
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 4
        height: parent.height - 4
        border.color: "black"
        radius: 10
        gradient: Gradient {
            GradientStop{position: 0; color: gradientColorBegin}
            GradientStop{position: 1; color: gradientColorEnd}
        }
    }

    Row {
        id: row

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        height: parent.height

        Text {
            anchors.verticalCenter: row.verticalCenter
            text: name
        }

    }

    MouseArea {
        anchors.fill: box
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            if (view.currentItem) {
                view.currentItem.setGradientColorUnselected();
            }
            countCallsDelegate.selected();
            setGradientColorSelected();
            view.currentIndex = model.index;
        }
    }
}

