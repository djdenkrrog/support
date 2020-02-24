import QtQuick 2.3
import QtQuick.XmlListModel 2.0

Rectangle {
    id: chartStatic
    anchors.fill: parent
    anchors.margins: 5

    property var modelXml: null
    property var listView: null
    property bool drawChart: true

    function updateCanvas()
    {
        canvas.requestPaint();
    }

    Canvas {
        id: canvas

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 30

        property int tickMargin: 50

        property real yGridStep: height / 12
        property real yGridOffset: height / 26

        function drawBackground(ctx)
        {
            ctx.save();
            ctx.fillStyle = "#ffffff";
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            ctx.strokeStyle = "#d7d7d7";
            ctx.beginPath();

            // Horizontal grid lines
            for (var i = 0; i < 12; i++) {
                ctx.moveTo(0, canvas.yGridOffset + i * canvas.yGridStep);
                ctx.lineTo(canvas.width, canvas.yGridOffset + i * canvas.yGridStep);
            }
            ctx.stroke();

            //Coordinate axes
            ctx.strokeStyle = "#666666";
            ctx.beginPath();

            var xStart = canvas.width - tickMargin;
            ctx.moveTo(xStart, 0);
            ctx.lineTo(xStart, canvas.height);
            for (i = 0; i < 12; i++) {
                ctx.moveTo(xStart, canvas.yGridOffset + i * canvas.yGridStep);
                ctx.lineTo(canvas.width, canvas.yGridOffset + i * canvas.yGridStep);
            }
            ctx.moveTo(0, canvas.yGridOffset + 11 * canvas.yGridStep);
            ctx.lineTo(canvas.width, canvas.yGridOffset + 11 * canvas.yGridStep);
            ctx.closePath();
            ctx.stroke();

            ctx.restore()
        }

        function drawScales(ctx, high, low)
        {
            ctx.save();
            ctx.strokeStyle = "#888888";
            ctx.font = "12px Arial"
            ctx.beginPath();

            // countCalls on y-axis
            var x = canvas.width - tickMargin + 3;
            low = 0;
            var countCallsStep = (high - low) / 11;
            for (var i = 0; i < 12; i++) {
                var count = parseFloat(high - i * countCallsStep).toFixed(0);
                ctx.fillText(count, x, canvas.yGridOffset + (i * yGridStep - 3));
            }



            ctx.closePath();
            ctx.stroke();
            ctx.restore();
        }

        function drawRectCountCalls(ctx, usersCalls, highestCountCalls)
        {
            ctx.save();
            ctx.globalAlpha = 0.8;
            ctx.lineWidth = 0;
            ctx.beginPath();

            var rectMargin = 20;
            var xOffsetLeft = 15;
            var xOrigin = canvas.width - tickMargin;
            var yOrigin = canvas.yGridOffset + 11 * canvas.yGridStep;

            var rectWidth = (xOrigin - xOffsetLeft - usersCalls.length * rectMargin) / usersCalls.length;

            var x = xOffsetLeft;
            for (var i = 0; i < usersCalls.length; i++) {
                var y = yOrigin - usersCalls[i].countCalls *
                        ((yOrigin - canvas.yGridOffset)  / highestCountCalls);

                ctx.fillStyle = "red";
                if (chartStatic.listView && chartStatic.listView.currentItem &&
                        usersCalls[i].personalId === chartStatic.listView.currentItem.personalId) {
                    ctx.fillStyle = "blue";
                }


                ctx.fillRect(x, y, rectWidth, yOrigin - y);

                ctx.font = "bold 12px Arial";
                ctx.fillStyle = "blue";
                ctx.fillText(usersCalls[i].countCalls, x, y - 3);

                ctx.save();
                ctx.translate(x, yOrigin);
                ctx.rotate(-90 * Math.PI / 180);
                ctx.font = "14px Arial";
                ctx.fillStyle = "black";
                ctx.fillText(usersCalls[i].name, 0, -2);
                ctx.restore();


                x = x + rectWidth + rectMargin;
            }

            ctx.stroke();
            ctx.restore();

        }

        onPaint: {
            if (parent.modelXml.status !== XmlListModel.Ready)
                return;

            var ctx = canvas.getContext("2d");
            ctx.globalCompositeOperation = "source-over";
            ctx.lineWidth = 1;

            var highestCountCalls = 0;
            var lowestCountCalls = 0;
            var usersCalls = [];
            drawBackground(ctx);

            for (var i = 0; i < modelXml.count; i++ ) {
                usersCalls.push({
                                    name: modelXml.get(i).name,
                                    countCalls: modelXml.get(i).countcalls,
                                    personalId: modelXml.get(i).user
                                });
            }

            if (usersCalls.length > 0) {
                lowestCountCalls = usersCalls[0].countCalls;
            }
            for (i = 0; i < usersCalls.length; i++ ) {
                if (highestCountCalls < usersCalls[i].countCalls) {
                    highestCountCalls = usersCalls[i].countCalls;
                }

                if (lowestCountCalls > usersCalls[i].countCalls) {
                    lowestCountCalls = usersCalls[i].countCalls;
                }

            }

            if (highestCountCalls % 11) {
                highestCountCalls = (highestCountCalls * 1) + (11 - highestCountCalls % 11);
            }

            drawScales(ctx, highestCountCalls, lowestCountCalls);

            if (drawChart) {
                drawRectCountCalls(ctx, usersCalls, highestCountCalls);
            }
        }
    }
}

