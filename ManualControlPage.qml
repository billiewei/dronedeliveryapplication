import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtPositioning 5.2
import QtLocation 5.3
import QtQuick.Particles 2.0
import QtQml 2.2
import HKUST 1.0

Rectangle {
    width: parent? parent.width : 400
    height: parent? parent.height : 900
    visible: true
    color: "#FFF"

    function changeFlightMode(){
        if (return_on_button.checked) {
            manual_control_handler.set_mode_return()
        }
        else if (delivery_button.checked) {
            manual_control_handler.set_mode_auto_delivery()
        }
        else if (other_modes_button.checked) {
            if (manual_button.checked) {manual_control_handler.set_mode_manual()}
            else if (assist_button.checked) {
                if (alt_control_button.checked) {manual_control_handler.set_mode_assist_altctl()}
                else {manual_control_handler.set_mode_assist_posctl()}
            }
            else if (auto_button.checked) {
                if (mission_button.checked) {manual_control_handler.set_mode_auto_mission()}
                else {manual_control_handler.set_mode_auto_loiter()}
            }
        }
    }

    Button{
        id: home_button
        text: "Home"
        anchors.top: parent.top
        anchors.topMargin: page.height*0.01
        anchors.left: parent.left
        anchors.leftMargin: page.width*0.025
        onClicked: {
            opening_page.visible = true
            manual_control_page.visible = false
        }
    }
    Button {
        id: vendor_track_drone
        text: "Track Drone"
        anchors.top: parent.top
        anchors.topMargin: page.height*0.01
        anchors.left: parent.left
        anchors.leftMargin: home_button.x + home_button.width + page.width*0.02
        onClicked: {
            manual_control_page.visible = false
            if (vendor_handler.delivery != 0) {vendor_track_page.visible = true}
                else {drone_location_page.visible = true}
        }
    }
    Label {
        id: batterytextlabel
        text: if (((manual_control_handler.voltage - 13500) / 31).toFixed(1) > 0) {
                  ((manual_control_handler.voltage - 13500) / 31).toFixed(1) + "%"}
              else {"0%"}
        y: home_button.y
        anchors.right: parent.right
        anchors.rightMargin: page.width*0.04
        font.letterSpacing: 2
    }
    Text {
        id: raw_voltage
        text: manual_control_handler.voltage + "mV"
        y: home_button.y + batterytextlabel.height
        anchors.right: parent.right
        anchors.rightMargin: page.width*0.04
        font.letterSpacing: 2
    }
    Rectangle {
        id: remainingbatteryoutline
        anchors.right: parent.right
        anchors.rightMargin: page.width*0.2
        y: home_button.y
        height: home_button.height - 4
        width: remainingbatteryoutline.height*3
        border.width: 2
        border.color: "#000"
        color: "#00000000"
        radius: 3
    }
    Rectangle {
        id: remainingbatteryfill
        x: remainingbatteryoutline.x + 2
        y: remainingbatteryoutline.y + 2
        radius: 1
        height: remainingbatteryoutline.height - 4
        width: if (((manual_control_handler.voltage - 13500) / 31) > 0.0) {
            (remainingbatteryoutline.width - 4.0)*(((manual_control_handler.voltage - 13500) / 3100))}
               else {0}
        color: if (((manual_control_handler.voltage - 13500) / 31) >= 85.00) {"#65E01F"}
               else if (((manual_control_handler.voltage - 13500) / 31) >= 60.00) {"#FF790A"}
               else if (((manual_control_handler.voltage - 13500) / 31) < 60.00) {"#D60000"}
    }
    Rectangle {
        id: toprowrectangle
        color: "#F2F2F2"
        height: computercontrollabel.height + armingstatelabel.height + lat_label.height + page.height*0.03
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: page.height*0.07
        width: page.width*0.95
        radius: 5
        Label {
            id: computercontrollabel
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            anchors.top: parent.top
            anchors.topMargin: page.height*0.01
            text: if (computertoggle.checked == false) {"Computer Control:  OFF"}
                  else {"Computer Control:  ON"}
        }
        Switch {
            id: computertoggle
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
            anchors.top: parent.top
            anchors.topMargin: page.height*0.01
            checked: false
            onCheckedChanged: if (computertoggle.checked == false) {manual_control_handler.stopTimer()}
                              else {manual_control_handler.startTimer()}
        }
        Label {
            id: armingstatelabel
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            anchors.top: computercontrollabel.bottom
            anchors.topMargin: page.height*0.01
            text: if (armingstatetoggle.checked == false) {"Arming State:  OFF"}
                  else {"Arming State:  ON"}
        }
        Switch {
            id: armingstatetoggle
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
            anchors.top: computercontrollabel.bottom
            anchors.topMargin: page.height*0.01
            checked: false
            onCheckedChanged: manual_control_handler.sendArmCommand(checked)
        }
        Text {
            id: lat_label
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            anchors.top: armingstatelabel.bottom
            anchors.topMargin: page.height*0.01
            text: "Latitude: " + manual_control_handler.latitude.toFixed(7)
        }
        Text {
            id: lon_label
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.4
            anchors.top: armingstatelabel.bottom
            anchors.topMargin: page.height*0.01
            text: "Longitude: " + manual_control_handler.longitude.toFixed(7)
        }
    }

    Rectangle {
        id: networkrectangle
        color: "#F2F2F2"
        y: toprowrectangle.y + toprowrectangle.height + page.height*0.01
        height: ip_label.height + port_label.height + connect_group.height + page.height*0.08
        anchors.horizontalCenter: parent.horizontalCenter
        width: page.width*0.95
        radius: 5
        Label {
            id: ip_label
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.08
            anchors.top: parent.top
            anchors.topMargin: page.height*0.02
            text: "IP Address"
        }
        TextField {
            id: ip_txt_field
            y: ip_label.y
            anchors.left: parent.left
            anchors.leftMargin: ip_label.x + ip_label.width + page.width*0.05
            placeholderText: "Please enter in IP Address"
            width: parent.width*0.6
        }
        Label {
            id: port_label
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.08
            anchors.top: ip_label.bottom
            anchors.topMargin: page.height*0.02
            text: "Port"
        }
        TextField {
            id: port_txt_field
            y: port_label.y
            anchors.left: parent.left
            anchors.leftMargin: ip_label.x + ip_label.width + page.width*0.05
            placeholderText: "Please enter the port number"
            width: parent.width*0.6
        }
        RowLayout {
            id: connect_group
            x: ip_label.x
            y: port_txt_field.y + port_txt_field.height + page.height*0.02
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                text: "Connect"
                isDefault: true
                onClicked: {
                    manual_control_handler.setHostAddress(ip_txt_field.text)
                    manual_control_handler.setPort(port_txt_field.text)
                    manual_control_handler.connectServer()
                }
            }

            Button {
                text: "Disconnect"
                onClicked: manual_control_handler.disconnectServer()
            }

            Label { text: manual_control_handler.status}
        }
    }
    Rectangle {
        id: controlsliders
        y: networkrectangle.y + networkrectangle.height + page.height*0.015
        width: page.width*0.95
        height: page.height*0.28
        anchors.horizontalCenter: parent.horizontalCenter
        color: toprowrectangle.color
        radius: 5

        // Z: Throttle
        Label {
            id:throttle_lable
            text: "Throttle"
            anchors.top: parent.top
            anchors.topMargin: page.height*0.03
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            font.pixelSize: page.height*0.015
            font.letterSpacing: 2
        }
        Slider {
            id: z_slider
            anchors.left: parent.left
            anchors.leftMargin: back_forward_label.width + page.width*0.05
            anchors.top: parent.top
            anchors.topMargin: page.height*0.03
            tickmarksEnabled: false
            updateValueWhileDragging: true
            value: 0
            minimumValue : 0
            maximumValue: 1000
            stepSize: 5
            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: controlsliders.width*0.6
                    implicitHeight: 8
                    color: "gray"
                    radius: 8
                }
            }
           onValueChanged: manual_control_handler.setZ(value)

        }
        Text {
            text: z_slider.value
            anchors.top: parent.top
            anchors.topMargin: page.height*0.03
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
        }

        // X: Back-Forward
        Label {
            id:back_forward_label
            text: "Back-Forward"
            anchors.top: parent.top
            anchors.topMargin: page.height*0.1
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            font.pixelSize: page.height*0.015
            font.letterSpacing: 2
        }
        Slider {
            id: x_slider
            anchors.left: parent.left
            anchors.leftMargin: back_forward_label.width + page.width*0.05
            anchors.top: parent.top
            anchors.topMargin: page.height*0.1
            tickmarksEnabled: false
            updateValueWhileDragging: true
            value: 0
            minimumValue : -1000
            maximumValue: 1000
            stepSize: 5.0
            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: controlsliders.width*0.6
                    implicitHeight: 8
                    color: "gray"
                    radius: 8
                }
            }
            onValueChanged: manual_control_handler.setX(value)
            onPressedChanged: value = 0
        }
        Text {
            text: x_slider.value
            anchors.top: parent.top
            anchors.topMargin: page.height*0.1
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
        }

        // Y: Left-Right
        Label {
            text: "Left-Right"
            anchors.top: parent.top
            anchors.topMargin: page.height*0.17
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            font.pixelSize: page.height*0.015
            font.letterSpacing: 2
        }
        Slider {
            id: y_slider
            anchors.left: parent.left
            anchors.leftMargin: back_forward_label.width + page.width*0.05
            anchors.top: parent.top
            anchors.topMargin: page.height*0.17
            tickmarksEnabled: false
            updateValueWhileDragging: true
            value: 0
            minimumValue : -1000
            maximumValue: 1000
            stepSize: 5
            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: controlsliders.width*0.6
                    implicitHeight: 8
                    color: "gray"
                    radius: 8
                }
            }
            onValueChanged: manual_control_handler.setY(value)
            onPressedChanged: value = 0
        }
        Text {
            text: y_slider.value
            anchors.top: parent.top
            anchors.topMargin: page.height*0.17
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
        }

        //R: yaw
        Label {
            text: "Yaw"
            anchors.top: parent.top
            anchors.topMargin: page.height*0.24
            anchors.left: parent.left
            anchors.leftMargin: page.width*0.04
            font.pixelSize: page.height*0.015
            font.letterSpacing: 2
        }
        Slider {
            id: r_slider
            anchors.left: parent.left
            anchors.leftMargin: back_forward_label.width + page.width*0.05
            anchors.top: parent.top
            anchors.topMargin: page.height*0.24
            tickmarksEnabled: false
            updateValueWhileDragging: true
            value: 0
            minimumValue : -1000
            maximumValue: 1000
            stepSize: 5
            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: controlsliders.width*0.6
                    implicitHeight: 8
                    color: "gray"
                    radius: 8
                }
            }
            onValueChanged: manual_control_handler.setR(value)
            onPressedChanged: value = 0
        }
        Text {
            text: r_slider.value
            anchors.top: parent.top
            anchors.topMargin: page.height*0.24
            anchors.right: parent.right
            anchors.rightMargin: page.width*0.04
        }

    }
    Rectangle {
            id: flightmode
            y: controlsliders.y + controlsliders.height + page.height*0.02
            width: page.width*0.95
            height: page.height*0.3
            anchors.horizontalCenter: parent.horizontalCenter
            color: toprowrectangle.color
            radius: 5
            GroupBox {
                id: returnbox
                anchors.left: parent.left
                anchors.leftMargin: page.width*0.03
                anchors.right: parent.right
                anchors.rightMargin: page.width*0.03
                anchors.top: parent.top
                anchors.topMargin: page.height*0.015
                height: flightmode.height*0.2
                RowLayout {
                    anchors.fill: parent
                    spacing: 10
                    ExclusiveGroup { id: returnGroup }
                    RadioButton {
                        id: return_on_button
                        text: "Return On"
                        checked: false
                        exclusiveGroup: returnGroup
                        onClicked: changeFlightMode()
                    }
                    RadioButton {
                        id: delivery_button
                        text: "Delivery"
                        checked: false
                        exclusiveGroup: returnGroup
                        onClicked: changeFlightMode()

                    }
                    RadioButton {
                        id: other_modes_button
                        text: "Other Modes"
                        checked: true
                        exclusiveGroup: returnGroup
                        onClicked: changeFlightMode()
                    }
                }
            }
            GroupBox {
                id: modebox
                anchors.left: parent.left
                anchors.leftMargin: page.width*0.03
                anchors.right: parent.right
                anchors.rightMargin: page.width*0.03
                y: returnbox.y + returnbox.height + page.height*0.01
                width: returnbox.width
                enabled: other_modes_button.checked
                height: flightmode.height*0.2
                RowLayout {
                    anchors.fill: parent
                    ExclusiveGroup { id: modeGroup }
                    RadioButton {
                        id: manual_button;
                        text: "Manual";
                        exclusiveGroup: modeGroup;
                        checked: true
                        onClicked: changeFlightMode()
                    }
                    RadioButton {
                        id: assist_button;
                        text: "Assist";
                        checked: false
                        exclusiveGroup: modeGroup;
                        onClicked: changeFlightMode()
                    }
                    RadioButton {
                        id: auto_button;
                        text: "Auto";
                        exclusiveGroup: modeGroup;
                        checked: false;
                        onClicked: changeFlightMode()
                    }
                }
            }
            GroupBox {
                id: assistbox
                anchors.right: parent.right
                anchors.rightMargin: page.width*0.03
                anchors.left: parent.left
                anchors.leftMargin: page.width*0.03
                y: modebox.y + modebox.height + page.height*0.01
                width: returnbox.width
                enabled: assist_button.checked
                height: flightmode.height*0.2
                RowLayout {
                    anchors.fill: parent
                    ExclusiveGroup { id: assistGroup }
                    RadioButton {
                        id: alt_control_button
                        text: "Alt Control";
                        checked: false
                        exclusiveGroup: assistGroup;
                        onClicked: changeFlightMode()
                    }
                    RadioButton {
                        id: pos_control_button
                        text: "Pos Control";
                        checked: true
                        exclusiveGroup: assistGroup;
                        onClicked: changeFlightMode()
                    }
                }
            }
            GroupBox {
                id: autobox
                anchors.left: parent.left
                anchors.leftMargin: page.width*0.03
                anchors.right: parent.right
                anchors.rightMargin: page.width*0.03
                y: assistbox.y + assistbox.height + page.height*0.01
                width: returnbox.width
                enabled: auto_button.checked
                height: flightmode.height*0.2
                RowLayout {
                    anchors.fill: parent
                    ExclusiveGroup { id: autoGroup }
                    RadioButton {
                        id: mission_button
                        text: "Mission";
                        checked: false;
                        exclusiveGroup: autoGroup;
                        onClicked: changeFlightMode()
                    }
                    RadioButton {
                        id: loiter_button
                        text: "Loiter";
                        checked: true;
                        exclusiveGroup: autoGroup
                        onClicked: changeFlightMode()
                    }
                }
            }
        }
}
