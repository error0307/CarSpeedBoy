import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: speedDisplay
    
    property real currentSpeed: 0.0
    property string currentExpression: "RELAXED"
    property string speedUnit: "km/h"
    property color backgroundColor: "#2d2d2d"
    property color textColor: "#ffffff"
    property color accentColor: "#00ff00"
    
    color: backgroundColor
    radius: 10
    border.color: "#404040"
    border.width: 2
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10
        
        // Title
        Text {
            text: "CURRENT SPEED"
            font.pixelSize: 18
            font.bold: true
            color: textColor
            opacity: 0.7
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Speed value
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Text {
                id: speedValue
                anchors.centerIn: parent
                text: Math.round(currentSpeed)
                font.pixelSize: 120
                font.bold: true
                color: getSpeedColor()
                
                function getSpeedColor() {
                    if (currentSpeed <= 20) return "#00ff00"      // Green - RELAXED
                    if (currentSpeed <= 60) return "#00ccff"      // Cyan - NORMAL
                    if (currentSpeed <= 100) return "#ffaa00"     // Orange - ALERT
                    if (currentSpeed <= 120) return "#ff6600"     // Dark Orange - WARNING
                    return "#ff0000"                               // Red - SCARED
                }
                
                // Smooth animation
                Behavior on text {
                    NumberAnimation { duration: 200 }
                }
            }
        }
        
        // Unit
        Text {
            text: speedUnit
            font.pixelSize: 24
            font.bold: true
            color: textColor
            opacity: 0.9
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Speed bar indicator
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#1a1a1a"
            radius: 5
            border.color: "#404040"
            border.width: 1
            
            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 2
                width: Math.min(parent.width - 4, (currentSpeed / 150) * (parent.width - 4))
                radius: 3
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#00ff00" }
                    GradientStop { position: 0.5; color: "#ffaa00" }
                    GradientStop { position: 1.0; color: "#ff0000" }
                }
                
                Behavior on width {
                    NumberAnimation { duration: 300; easing.type: Easing.OutQuad }
                }
            }
        }
        
        // Speed range labels
        RowLayout {
            Layout.fillWidth: true
            spacing: 0
            
            Text {
                text: "0"
                font.pixelSize: 12
                color: textColor
                opacity: 0.6
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: "75"
                font.pixelSize: 12
                color: textColor
                opacity: 0.6
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: "150+"
                font.pixelSize: 12
                color: textColor
                opacity: 0.6
            }
        }
    }
}
