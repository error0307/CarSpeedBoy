import QtQuick 2.15import QtQuick 2.15

import QtQuick.Controls 2.15import QtQuick.Controls 2.15

import QtQuick.Layouts 1.15import QtQuick.Layouts 1.15



ApplicationWindow {ApplicationWindow {

    id: settingsDialog    id: settingsDialog

    title: "CarSpeedBoy Settings"    title: "CarSpeedBoy Settings"

    width: 600    width: 600

    height: 550    height: 550

    modality: Qt.ApplicationModal    modality: Qt.ApplicationModal

    flags: Qt.Dialog    flags: Qt.Dialog

    visible: false    visible: false

        

    // Settings values    // Settings values

    property real relaxedMax: 20.0    property real relaxedMax: 20.0

    property real normalMax: 60.0    property real normalMax: 60.0

    property real alertMax: 100.0    property real alertMax: 100.0

    property real warningMax: 120.0    property real warningMax: 120.0

    property bool loggingEnabled: true    property bool loggingEnabled: true

    property string theme: "dark"    property string theme: "dark"

        

    signal accepted()    signal accepted()

    signal rejected()    signal rejected()

        

    function open() {    function open() {

        visible = true        visible = true

    }    }

        

    function close() {    function close() {

        visible = false        visible = false

    }    }

        

    function applySettings() {    function applySettings() {

        console.log("Settings applied:")        console.log("Settings applied:")

        console.log("  Relaxed max:", relaxedMax)        console.log("  Relaxed max:", relaxedMax)

        console.log("  Normal max:", normalMax)        console.log("  Normal max:", normalMax)

        console.log("  Alert max:", alertMax)        console.log("  Alert max:", alertMax)

        console.log("  Warning max:", warningMax)        console.log("  Warning max:", warningMax)

        console.log("  Logging:", loggingEnabled)        console.log("  Logging:", loggingEnabled)

        console.log("  Theme:", theme)        console.log("  Theme:", theme)

        accepted()        accepted()

        close()        close()

    }    }

        

    function resetSettings() {    function resetSettings() {

        relaxedMax = 20.0        relaxedMax = 20.0

        normalMax = 60.0        normalMax = 60.0

        alertMax = 100.0        alertMax = 100.0

        warningMax = 120.0        warningMax = 120.0

        loggingEnabled = true        loggingEnabled = true

        theme = "dark"        theme = "dark"

    }    }

        

    Rectangle {    Rectangle {

        anchors.fill: parent        anchors.fill: parent

        color: "#2d2d2d"        color: "#2d2d2d"

                

        ScrollView {        ScrollView {

            anchors.fill: parent            anchors.fill: parent

            anchors.margins: 20            anchors.margins: 20

            clip: true            clip: true

                        

            ColumnLayout {            ColumnLayout {

                width: parent.width - 40                width: parent.width - 40

                spacing: 20                spacing: 20

                                

                // Speed Thresholds Section                // Speed Thresholds Section

                GroupBox {                GroupBox {

                    Layout.fillWidth: true                    Layout.fillWidth: true

                    title: "Speed Thresholds (km/h)"                    title: "Speed Thresholds (km/h)"

                                        

                    background: Rectangle {                    background: Rectangle {

                        color: "#1a1a1a"                        color: "#1a1a1a"

                        border.color: "#404040"                        border.color: "#404040"

                        radius: 5                        radius: 5

                    }                    }

                                        

                    label: Label {                    label: Label {

                        text: parent.title                        text: parent.title

                        color: "#ffffff"                        color: "#ffffff"

                        font.bold: true                        font.bold: true

                        padding: 5                        padding: 5

                    }                    }

                                        

                    ColumnLayout {                    ColumnLayout {

                        anchors.fill: parent                        anchors.fill: parent

                        spacing: 15                        spacing: 15

                                                

                        // Relaxed threshold                        // Relaxed threshold

                        RowLayout {                        RowLayout {

                            Layout.fillWidth: true                            Layout.fillWidth: true

                                                        

                            Label {                            Label {

                                text: "Relaxed (0 - " + relaxedMax.toFixed(0) + ")"                                text: "Relaxed (0 - " + relaxedMax.toFixed(0) + ")"

                                color: "#00ff00"                                color: "#00ff00"

                                Layout.preferredWidth: 150                                Layout.preferredWidth: 150

                            }                            }

                                                        

                            Slider {                            Slider {

                                Layout.fillWidth: true                                Layout.fillWidth: true

                                from: 10                                from: 10

                                to: 40                                to: 40

                                value: relaxedMax                                value: relaxedMax

                                stepSize: 1                                stepSize: 1

                                onValueChanged: relaxedMax = value                                onValueChanged: relaxedMax = value

                            }                            }

                                                        

                            SpinBox {                            SpinBox {

                                from: 10                                from: 10

                                to: 40                                to: 40

                                value: relaxedMax                                value: relaxedMax

                                onValueModified: relaxedMax = value                                onValueModified: relaxedMax = value

                            }                            }

                        }                        }

                                                

                        // Normal threshold                        // Normal threshold

                        RowLayout {                        RowLayout {

                            Layout.fillWidth: true                            Layout.fillWidth: true

                                                        

                            Label {                            Label {

                                text: "Normal (" + (relaxedMax + 1).toFixed(0) + " - " + normalMax.toFixed(0) + ")"                                text: "Normal (" + (relaxedMax + 1).toFixed(0) + " - " + normalMax.toFixed(0) + ")"

                                color: "#00ccff"                                color: "#00ccff"

                                Layout.preferredWidth: 150                                Layout.preferredWidth: 150

                            }                            }

                                                        

                            Slider {                            Slider {

                                Layout.fillWidth: true                                Layout.fillWidth: true

                                from: 30                                from: 30

                                to: 80                                to: 80

                                value: normalMax                                value: normalMax

                                stepSize: 1                                stepSize: 1

                                onValueChanged: normalMax = value                                onValueChanged: normalMax = value

                            }                            }

                                                        

                            SpinBox {                            SpinBox {

                                from: 30                                from: 30

                                to: 80                                to: 80

                                value: normalMax                                value: normalMax

                                onValueModified: normalMax = value                                onValueModified: normalMax = value

                            }                            }

                        }                        }

                                                

                        // Alert threshold                        // Alert threshold

                        RowLayout {                        RowLayout {

                            Layout.fillWidth: true                            Layout.fillWidth: true

                                                        

                            Label {                            Label {

                                text: "Alert (" + (normalMax + 1).toFixed(0) + " - " + alertMax.toFixed(0) + ")"                                text: "Alert (" + (normalMax + 1).toFixed(0) + " - " + alertMax.toFixed(0) + ")"

                                color: "#ffaa00"                                color: "#ffaa00"

                                Layout.preferredWidth: 150                                Layout.preferredWidth: 150

                            }                            }

                                                        

                            Slider {                            Slider {

                                Layout.fillWidth: true                                Layout.fillWidth: true

                                from: 70                                from: 70

                                to: 120                                to: 120

                                value: alertMax                                value: alertMax

                                stepSize: 1                                stepSize: 1

                                onValueChanged: alertMax = value                                onValueChanged: alertMax = value

                            }                            }

                                                        

                            SpinBox {                            SpinBox {

                                from: 70                                from: 70

                                to: 120                                to: 120

                                value: alertMax                                value: alertMax

                                onValueModified: alertMax = value                                onValueModified: alertMax = value

                            }                            }

                        }                        }

                                                

                        // Warning threshold                        // Warning threshold

                        RowLayout {                        RowLayout {

                            Layout.fillWidth: true                            Layout.fillWidth: true

                                                        

                            Label {                            Label {

                                text: "Warning (" + (alertMax + 1).toFixed(0) + " - " + warningMax.toFixed(0) + ")"                                text: "Warning (" + (alertMax + 1).toFixed(0) + " - " + warningMax.toFixed(0) + ")"

                                color: "#ff6600"                                color: "#ff6600"

                                Layout.preferredWidth: 150                                Layout.preferredWidth: 150

                            }                            }

                                                        

                            Slider {                            Slider {

                                Layout.fillWidth: true                                Layout.fillWidth: true

                                from: 100                                from: 100

                                to: 150                                to: 150

                                value: warningMax                                value: warningMax

                                stepSize: 1                                stepSize: 1

                                onValueChanged: warningMax = value                                onValueChanged: warningMax = value

                            }                            }

                                                        

                            SpinBox {                            SpinBox {

                                from: 100                                from: 100

                                to: 150                                to: 150

                                value: warningMax                                value: warningMax

                                onValueModified: warningMax = value                                onValueModified: warningMax = value

                            }                            }

                        }                        }

                                                

                        Label {                        Label {

                            text: "Scared: " + (warningMax + 1).toFixed(0) + " +"                            text: "Scared: " + (warningMax + 1).toFixed(0) + " +"

                            color: "#ff0000"                            color: "#ff0000"

                            font.bold: true                            font.bold: true

                        }                        }

                    }                    }

                }                }

                                

                // Display Settings Section                // Display Settings Section

                GroupBox {                GroupBox {

                    Layout.fillWidth: true                    Layout.fillWidth: true

                    title: "Display Settings"                    title: "Display Settings"

                                        

                    background: Rectangle {                    background: Rectangle {

                        color: "#1a1a1a"                        color: "#1a1a1a"

                        border.color: "#404040"                        border.color: "#404040"

                        radius: 5                        radius: 5

                    }                    }

                                        

                    label: Label {                    label: Label {

                        text: parent.title                        text: parent.title

                        color: "#ffffff"                        color: "#ffffff"

                        font.bold: true                        font.bold: true

                        padding: 5                        padding: 5

                    }                    }

                                        

                    ColumnLayout {                    ColumnLayout {

                        anchors.fill: parent                        anchors.fill: parent

                        spacing: 15                        spacing: 15

                                                

                        RowLayout {                        RowLayout {

                            Label {                            Label {

                                text: "Theme:"                                text: "Theme:"

                                color: "#cccccc"                                color: "#cccccc"

                                Layout.preferredWidth: 100                                Layout.preferredWidth: 100

                            }                            }

                                                        

                            ComboBox {                            ComboBox {

                                Layout.fillWidth: true                                Layout.fillWidth: true

                                model: ["Dark", "Light"]                                model: ["Dark", "Light"]

                                currentIndex: theme === "dark" ? 0 : 1                                currentIndex: theme === "dark" ? 0 : 1

                                onCurrentTextChanged: {                                onCurrentTextChanged: {

                                    theme = currentText.toLowerCase()                                    theme = currentText.toLowerCase()

                                }                                }

                            }                            }

                        }                        }

                                                

                        CheckBox {                        CheckBox {

                            text: "Enable Data Logging"                            text: "Enable Data Logging"

                            checked: loggingEnabled                            checked: loggingEnabled

                            onCheckedChanged: loggingEnabled = checked                            onCheckedChanged: loggingEnabled = checked

                                                        

                            contentItem: Text {                            contentItem: Text {

                                text: parent.text                                text: parent.text

                                color: "#cccccc"                                color: "#cccccc"

                                leftPadding: parent.indicator.width + parent.spacing                                leftPadding: parent.indicator.width + parent.spacing

                                verticalAlignment: Text.AlignVCenter                                verticalAlignment: Text.AlignVCenter

                            }                            }

                        }                        }

                    }                    }

                }                }

            }            }

        }        }

    }    }

        

    // Footer with buttons    // Footer with buttons

    footer: Rectangle {    footer: Rectangle {

        width: parent.width        width: parent.width

        height: 60        height: 60

        color: "#1a1a1a"        color: "#1a1a1a"

                

        RowLayout {        RowLayout {

            anchors.fill: parent            anchors.fill: parent

            anchors.margins: 15            anchors.margins: 15

            spacing: 10            spacing: 10

                        

            Item { Layout.fillWidth: true }            Item { Layout.fillWidth: true }

                        

            Button {            Button {

                text: "Reset"                text: "Reset"

                Layout.preferredWidth: 100                Layout.preferredWidth: 100

                onClicked: resetSettings()                onClicked: resetSettings()

                                

                background: Rectangle {                background: Rectangle {

                    color: parent.hovered ? "#555555" : "#404040"                    color: parent.hovered ? "#555555" : "#404040"

                    border.color: "#666666"                    border.color: "#666666"

                    radius: 5                    radius: 5

                }                }

                                

                contentItem: Text {                contentItem: Text {

                    text: parent.text                    text: parent.text

                    color: "#ffffff"                    color: "#ffffff"

                    horizontalAlignment: Text.AlignHCenter                    horizontalAlignment: Text.AlignHCenter

                    verticalAlignment: Text.AlignVCenter                    verticalAlignment: Text.AlignVCenter

                }                }

            }            }

                        

            Button {            Button {

                text: "Cancel"                text: "Cancel"

                Layout.preferredWidth: 100                Layout.preferredWidth: 100

                onClicked: {                onClicked: {

                    rejected()                    rejected()

                    close()                    close()

                }                }

                                

                background: Rectangle {                background: Rectangle {

                    color: parent.hovered ? "#555555" : "#404040"                    color: parent.hovered ? "#555555" : "#404040"

                    border.color: "#666666"                    border.color: "#666666"

                    radius: 5                    radius: 5

                }                }

                                

                contentItem: Text {                contentItem: Text {

                    text: parent.text                    text: parent.text

                    color: "#ffffff"                    color: "#ffffff"

                    horizontalAlignment: Text.AlignHCenter                    horizontalAlignment: Text.AlignHCenter

                    verticalAlignment: Text.AlignVCenter                    verticalAlignment: Text.AlignVCenter

                }                }

            }            }

                        

            Button {            Button {

                text: "OK"                text: "OK"

                Layout.preferredWidth: 100                Layout.preferredWidth: 100

                onClicked: applySettings()                onClicked: applySettings()

                                

                background: Rectangle {                background: Rectangle {

                    color: parent.hovered ? "#00aa00" : "#008800"                    color: parent.hovered ? "#00aa00" : "#008800"

                    border.color: "#00ff00"                    border.color: "#00ff00"

                    radius: 5                    radius: 5

                }                }

                                

                contentItem: Text {                contentItem: Text {

                    text: parent.text                    text: parent.text

                    color: "#ffffff"                    color: "#ffffff"

                    horizontalAlignment: Text.AlignHCenter                    horizontalAlignment: Text.AlignHCenter

                    verticalAlignment: Text.AlignVCenter                    verticalAlignment: Text.AlignVCenter

                    font.bold: true                    font.bold: true

                }                }

            }            }

        }        }

    }    }

}}


        
        ScrollView {
            anchors.fill: parent
            anchors.margins: 20
            clip: true
            
            ColumnLayout {
                width: parent.width
                spacing: 20
                
                // Speed Thresholds Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "Speed Thresholds (km/h)"
                    
                    background: Rectangle {
                        color: "#1a1a1a"
                        border.color: "#404040"
                        radius: 5
                    }
                    
                    label: Label {
                        text: parent.title
                        color: "#ffffff"
                        font.bold: true
                        padding: 5
                    }
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15
                        
                        // Relaxed threshold
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Relaxed (0 - " + relaxedMax.toFixed(0) + ")"
                                color: "#00ff00"
                                Layout.preferredWidth: 150
                            }
                            
                            Slider {
                                id: relaxedSlider
                                Layout.fillWidth: true
                                from: 10
                                to: 40
                                value: relaxedMax
                                stepSize: 1
                                onValueChanged: relaxedMax = value
                            }
                            
                            SpinBox {
                                from: 10
                                to: 40
                                value: relaxedMax
                                onValueModified: relaxedMax = value
                            }
                        }
                        
                        // Normal threshold
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Normal (" + (relaxedMax + 1).toFixed(0) + " - " + normalMax.toFixed(0) + ")"
                                color: "#00ccff"
                                Layout.preferredWidth: 150
                            }
                            
                            Slider {
                                Layout.fillWidth: true
                                from: 30
                                to: 80
                                value: normalMax
                                stepSize: 1
                                onValueChanged: normalMax = value
                            }
                            
                            SpinBox {
                                from: 30
                                to: 80
                                value: normalMax
                                onValueModified: normalMax = value
                            }
                        }
                        
                        // Alert threshold
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Alert (" + (normalMax + 1).toFixed(0) + " - " + alertMax.toFixed(0) + ")"
                                color: "#ffaa00"
                                Layout.preferredWidth: 150
                            }
                            
                            Slider {
                                Layout.fillWidth: true
                                from: 70
                                to: 120
                                value: alertMax
                                stepSize: 1
                                onValueChanged: alertMax = value
                            }
                            
                            SpinBox {
                                from: 70
                                to: 120
                                value: alertMax
                                onValueModified: alertMax = value
                            }
                        }
                        
                        // Warning threshold
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                text: "Warning (" + (alertMax + 1).toFixed(0) + " - " + warningMax.toFixed(0) + ")"
                                color: "#ff6600"
                                Layout.preferredWidth: 150
                            }
                            
                            Slider {
                                Layout.fillWidth: true
                                from: 100
                                to: 150
                                value: warningMax
                                stepSize: 1
                                onValueChanged: warningMax = value
                            }
                            
                            SpinBox {
                                from: 100
                                to: 150
                                value: warningMax
                                onValueModified: warningMax = value
                            }
                        }
                        
                        Label {
                            text: "Scared: " + (warningMax + 1).toFixed(0) + " +"
                            color: "#ff0000"
                            font.bold: true
                        }
                    }
                }
                
                // Display Settings Section
                GroupBox {
                    Layout.fillWidth: true
                    title: "Display Settings"
                    
                    background: Rectangle {
                        color: "#1a1a1a"
                        border.color: "#404040"
                        radius: 5
                    }
                    
                    label: Label {
                        text: parent.title
                        color: "#ffffff"
                        font.bold: true
                        padding: 5
                    }
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15
                        
                        RowLayout {
                            Label {
                                text: "Theme:"
                                color: "#cccccc"
                                Layout.preferredWidth: 100
                            }
                            
                            ComboBox {
                                id: themeCombo
                                Layout.fillWidth: true
                                model: ["Dark", "Light"]
                                currentIndex: theme === "dark" ? 0 : 1
                                onCurrentTextChanged: {
                                    theme = currentText.toLowerCase()
                                }
                            }
                        }
                        
                        CheckBox {
                            text: "Enable Data Logging"
                            checked: loggingEnabled
                            onCheckedChanged: loggingEnabled = checked
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#cccccc"
                                leftPadding: parent.indicator.width + parent.spacing
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
                
                // Info section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: "#1a1a1a"
                    radius: 5
                    border.color: "#404040"
                    
                    Text {
                        anchors.centerIn: parent
                        anchors.margins: 10
                        width: parent.width - 20
                        text: "💡 Tip: Adjust thresholds to customize when the character's expression changes based on your speed."
                        color: "#888888"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
    
    // Footer with buttons
    footer: Rectangle {
        width: parent.width
        height: 60
        color: "#1a1a1a"
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Reset"
                Layout.preferredWidth: 100
                onClicked: resetSettings()
                
                background: Rectangle {
                    color: parent.hovered ? "#555555" : "#404040"
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
            
            Button {
                text: "Cancel"
                Layout.preferredWidth: 100
                onClicked: {
                    rejected()
                    close()
                }
                
                background: Rectangle {
                    color: parent.hovered ? "#555555" : "#404040"
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
            
            Button {
                text: "OK"
                Layout.preferredWidth: 100
                onClicked: applySettings()
                
                background: Rectangle {
                    color: parent.hovered ? "#00aa00" : "#008800"
                    border.color: "#00ff00"
                    radius: 5
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                }
            }
        }
    }
}
