import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.XmlListModel 2.0
import "./content"

Window {
    id: mainWindow
    visible: true
    width: 1024
    height: 768
    color: "grey"
    minimumWidth:  640
    minimumHeight: 480


    Rectangle {
        id: banner
        height: 80
        anchors.top: parent.top
        width: parent.width
        color: "#000000"

        Logo {
            id: logo

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: textItem.left
            width : parent.height; height: parent.height

            angle: 180
            yAxis: 1
            xAxis: 0
        }

        Item {
            id: textItem
            width: titleText.width + qmlText.width
            height: titleText.height + qmlText.height
            anchors.horizontalCenter: banner.horizontalCenter
            anchors.verticalCenter: banner.verticalCenter

            Text {
                id: titleText
                anchors.verticalCenter: textItem.verticalCenter
                color: "#ffffff"
                font.family: "Abel"
                font.pointSize: 40
                text: "Number of calls to the user "
            }
            Text {
                id: qmlText
                anchors.verticalCenter: textItem.verticalCenter
                anchors.left: titleText.right
                color: "#5caa15"
                font.family: "Abel"
                font.pointSize: 40
                text: "QML"
            }
        }
    }

    XmlListModel {
        id: xmlModel
        source: "./content/userscall.xml"
        query: "/calls/callInfo"

        XmlRole {name: "user"; query: "user_r/string()"}
        XmlRole {name: "name"; query: "name/string()"}
        XmlRole {name: "countcalls"; query: "countcalls/string()"}

        onStatusChanged: {
            if (status === XmlListModel.Ready) {
                chart.updateCanvas();
            }
        }
    }

    Component {
        id: countCallsComponent
        CountCallsDelegate {
            name: model.name
            personalId: model.user
            width: parent.width
            height: 30

            onSelected: {
                chart.updateCanvas();
            }
        }
    }

    Rectangle {
        id: recListView

        anchors.top: banner.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 300
        color : "lightgrey"
        border.width: 2
        border.color: "black"

        ListView {
            id: listView

            property var currentItem: null;

            anchors.fill: parent
            anchors.margins: 3
            spacing: 2
            highlight: Rectangle {
                color: "blue"
                radius: 10
            }
            highlightFollowsCurrentItem: false

            model: xmlModel
            delegate: countCallsComponent

            add: Transition {
                NumberAnimation {
                    properties: "x,y"; from: 100; duration: 1000
                }
            }
        }
    }

    Rectangle {
        id: recChart

        anchors.left: recListView.right
        anchors.right: parent.right
        anchors.top: banner.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 2
        color : "white"
        border.width: 2
        border.color: "black"

        ChartStatic {
            id: chart
            modelXml: xmlModel
            listView: listView
        }
    }
}
