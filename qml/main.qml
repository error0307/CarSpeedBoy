import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: mainWindow
    visible: true
    width: 1920
    height: 1080
    title: "CarSpeedBoy"
    color: "#000000"
    
    // Current application state (will be connected to C++ backend)
    property real currentSpeed: 0.0
    property string currentExpression: "RELAXED"
    property int rawSpeed: 0
    property string connectionStatus: "Disconnected"
    
    // Settings dialog (commented out for now due to QML issues)
    /*
    SettingsDialog {
        id: settingsDialog
        anchors.centerIn: parent
        
        // Load initial settings from config
        Component.onCompleted: {
            // TODO: Load from C++ ConfigurationManager
            relaxedMax = 20.0
            normalMax = 60.0
            alertMax = 100.0
            warningMax = 120.0
            loggingEnabled = true
            theme = "dark"
        }
        
        onAccepted: {
            // TODO: Save settings to C++ backend
            console.log("Settings saved")
        }
    }
    */
    
    // Main layout
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        // Header bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: "#1a1a1a"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20
                
                Text {
                    text: "CarSpeedBoy"
                    color: "#ffffff"
                    font.pixelSize: 32
                    font.bold: true
                }
                
                Item { Layout.fillWidth: true }
                
                // Connection status indicator
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 40
                    color: connectionStatus === "Connected" ? "#004400" : "#440000"
                    border.color: connectionStatus === "Connected" ? "#00ff00" : "#ff0000"
                    border.width: 2
                    radius: 20
                    
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 10
                        
                        Rectangle {
                            width: 12
                            height: 12
                            radius: 6
                            color: connectionStatus === "Connected" ? "#00ff00" : "#ff0000"
                            
                            SequentialAnimation on opacity {
                                running: connectionStatus !== "Connected"
                                loops: Animation.Infinite
                                NumberAnimation { to: 0.3; duration: 500 }
                                NumberAnimation { to: 1.0; duration: 500 }
                            }
                        }
                        
                        Text {
                            text: connectionStatus
                            color: "#ffffff"
                            font.pixelSize: 14
                        }
                    }
                }
                
                // Settings button
                Button {
                    text: "⚙️ Settings"
                    Layout.preferredHeight: 40
                    //onClicked: settingsDialog.open()  // Commented out - SettingsDialog disabled
                    enabled: false
                    
                    background: Rectangle {
                        color: parent.hovered ? "#404040" : "#2d2d2d"
                        border.color: "#666666"
                        radius: 5
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
        
        // Main content area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 40
                
                // Left panel - Character view
                CharacterView {
                    id: characterView
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: parent.width * 0.6
                    
                    currentExpression: mainWindow.currentExpression
                }
                
                // Right panel - Speed and info
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width * 0.35
                    spacing: 30
                    
                    // Speed display
                    SpeedDisplay {
                        id: speedDisplay
                        Layout.fillWidth: true
                        Layout.preferredHeight: 300
                        
                        currentSpeed: mainWindow.currentSpeed
                        currentExpression: mainWindow.currentExpression
                    }
                    
                    // Raw data display
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        color: "#1a1a1a"
                        radius: 10
                        border.color: "#333333"
                        border.width: 2
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 10
                            
                            Text {
                                text: "Vehicle Data"
                                color: "#ffffff"
                                font.pixelSize: 20
                                font.bold: true
                            }
                            
                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: "#333333"
                            }
                            
                            GridLayout {
                                Layout.fillWidth: true
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 20
                                
                                Text {
                                    text: "Raw Speed:"
                                    color: "#999999"
                                    font.pixelSize: 14
                                }
                                Text {
                                    text: rawSpeed + " km/h"
                                    color: "#00ff00"
                                    font.pixelSize: 14
                                    font.family: "monospace"
                                }
                                
                                Text {
                                    text: "Filtered Speed:"
                                    color: "#999999"
                                    font.pixelSize: 14
                                }
                                Text {
                                    text: currentSpeed.toFixed(1) + " km/h"
                                    color: "#00ccff"
                                    font.pixelSize: 14
                                    font.family: "monospace"
                                }
                                
                                Text {
                                    text: "Expression:"
                                    color: "#999999"
                                    font.pixelSize: 14
                                }
                                Text {
                                    text: currentExpression
                                    color: "#ffaa00"
                                    font.pixelSize: 14
                                    font.family: "monospace"
                                }
                            }
                        }
                    }
                    
                    Item { Layout.fillHeight: true }
                }
            }
        }
        
        // Footer bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#1a1a1a"
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                
                Text {
                    text: "© 2024 CarSpeedBoy | Automotive Grade Linux"
                    color: "#666666"
                    font.pixelSize: 12
                }
                
                Item { Layout.fillWidth: true }
                
                Text {
                    text: "Version 1.0.0"
                    color: "#666666"
                    font.pixelSize: 12
                }
            }
        }
    }
    
    // Demo mode - simulate speed changes for testing
    Timer {
        id: demoTimer
        interval: 100
        running: connectionStatus === "Disconnected" // Run demo when not connected
        repeat: true
        
        property real demoSpeed: 0
        property real speedDirection: 1
        
        onTriggered: {
            // Simulate speed changes
            demoSpeed += speedDirection * 2
            
            if (demoSpeed >= 140) {
                speedDirection = -1
            } else if (demoSpeed <= 0) {
                speedDirection = 1
            }
            
            mainWindow.rawSpeed = Math.round(demoSpeed)
            mainWindow.currentSpeed = demoSpeed
            
            // Update expression based on speed
            if (demoSpeed <= 20) {
                mainWindow.currentExpression = "RELAXED"
            } else if (demoSpeed <= 60) {
                mainWindow.currentExpression = "NORMAL"
            } else if (demoSpeed <= 100) {
                mainWindow.currentExpression = "ALERT"
            } else if (demoSpeed <= 120) {
                mainWindow.currentExpression = "WARNING"
            } else {
                mainWindow.currentExpression = "SCARED"
            }
        }
    }
}
