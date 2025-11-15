import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: characterView
    
    property string currentExpression: "RELAXED"  // Alias for external binding
    property string currentAnimationPath: ""
    property string currentState: currentExpression  // Sync with currentExpression
    property color backgroundColor: "#1a1a1a"
    
    color: backgroundColor
    radius: 10
    border.color: "#404040"
    border.width: 2
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // State label
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: stateText.width + 40
            Layout.preferredHeight: 40
            color: getStateColor()
            radius: 20
            
            function getStateColor() {
                switch(currentState) {
                    case "RELAXED": return "#00aa00"
                    case "NORMAL": return "#0099cc"
                    case "ALERT": return "#ff9900"
                    case "WARNING": return "#ff6600"
                    case "SCARED": return "#cc0000"
                    default: return "#666666"
                }
            }
            
            Text {
                id: stateText
                anchors.centerIn: parent
                text: currentState
                font.pixelSize: 20
                font.bold: true
                color: "#ffffff"
            }
            
            Behavior on color {
                ColorAnimation { duration: 300 }
            }
        }
        
        // Character animation area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            Rectangle {
                anchors.centerIn: parent
                width: Math.min(parent.width * 0.8, parent.height * 0.8)
                height: width
                color: "#2d2d2d"
                radius: 20
                border.color: "#404040"
                border.width: 2
                
                // Animation or placeholder
                Item {
                    anchors.fill: parent
                    anchors.margins: 20
                    
                    // AnimatedImage for GIF support
                    AnimatedImage {
                        id: characterAnimation
                        anchors.fill: parent
                        source: currentAnimationPath
                        fillMode: Image.PreserveAspectFit
                        playing: true
                        visible: currentAnimationPath !== ""
                        
                        onStatusChanged: {
                            if (status === Image.Error) {
                                console.warn("Failed to load animation:", currentAnimationPath)
                            }
                        }
                    }
                    
                    // Placeholder when no animation available
                    Column {
                        anchors.centerIn: parent
                        spacing: 20
                        visible: !characterAnimation.visible
                        
                        // Emoji-style character representation
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: getEmoji()
                            font.pixelSize: 120
                            
                            function getEmoji() {
                                switch(currentState) {
                                    case "RELAXED": return "😌"
                                    case "NORMAL": return "😊"
                                    case "ALERT": return "😮"
                                    case "WARNING": return "😰"
                                    case "SCARED": return "😱"
                                    default: return "🙂"
                                }
                            }
                            
                            // Pulse animation
                            SequentialAnimation on scale {
                                running: true
                                loops: Animation.Infinite
                                NumberAnimation { to: 1.1; duration: 800; easing.type: Easing.InOutQuad }
                                NumberAnimation { to: 1.0; duration: 800; easing.type: Easing.InOutQuad }
                            }
                        }
                        
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Animation: " + (currentAnimationPath || "Not loaded")
                            font.pixelSize: 14
                            color: "#888888"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
        
        // Expression description
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#2d2d2d"
            radius: 10
            border.color: "#404040"
            border.width: 1
            
            Text {
                anchors.centerIn: parent
                anchors.margins: 15
                width: parent.width - 30
                text: getDescription()
                font.pixelSize: 16
                color: "#cccccc"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                
                function getDescription() {
                    switch(currentState) {
                        case "RELAXED": return "Take it easy! 😌"
                        case "NORMAL": return "Cruising comfortably 🚗"
                        case "ALERT": return "Speed is rising... 👀"
                        case "WARNING": return "Slow down a bit! ⚠️"
                        case "SCARED": return "WAY TOO FAST! 🚨"
                        default: return "Ready to go!"
                    }
                }
            }
        }
    }
}
