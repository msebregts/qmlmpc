/*
 * Copyright 2015, Maarten Sebregts <maartensebregts AT gmail DOT com>
 *
 * This file is part of qmlmpc.
 *
 * qmlmpc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qmlmpc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qmlmpc. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import QtQuick.Window 2.0

import "mpc"
import "."
import "widgets"

Window {
    visible: true
    width: Style.totalWidth
    height: Style.totalHeight

    Rectangle {
        anchors.fill: parent
        color: "black"

        MpdButton {
            id: settingsButton
            text: "Settings"
            active: false
            onClicked: active = !active
            height: Style.topRowHeight
        }

        MpdPanel {
            width: parent.width
            height: parent.height - settingsButton.height
            y: settingsButton.height
            visible: !settingsButton.active
        }

        Rectangle {
            width: parent.width
            height: parent.height - settingsButton.height
            y: settingsButton.height
            visible: settingsButton.active
            onVisibleChanged: resetSettings()
            color: "black"

            Grid {
                id: settingsGrid
                anchors { centerIn: parent }
                columns: 2
                spacing: 5

                MpdText {
                    width: 100
                    height: 30
                    text: "Connection settings to MPD server"
                    font.pointSize: 16
                }
                Item{ width: 1; height: 1 }
                MpdText { width: 100; height: 30; text: "Host" }
                TddTextInput { id: mpdHost; width: 500 }
                MpdText { width: 100; height: 30; text: "Port" }
                TddTextInput { id: mpdPort; width: 500 }
                MpdText { width: 100; height: 30; text: "Password" }
                TddTextInput { id: mpdPassword; width: 500; textInput.echoMode: TextInput.Password }
                MpdButton {
                    text: "Save changes"
                    onClicked: saveSettings()
                }
                MpdButton {
                    text: "Discard changes"
                    onClicked: resetSettings()
                }
            }
        }
    }

    function resetSettings() {
        mpdHost.text = settings.value("mpd/host")
        mpdPort.text = settings.value("mpd/port")
        mpdPassword.text = settings.value("mpd/password")
    }
    function saveSettings() {
        settings.setValue("mpd/host", mpdHost.text)
        settings.setValue("mpd/port", mpdPort.text)
        settings.setValue("mpd/password", mpdPassword.text)
        mpdConnector.connection.reconnect(mpdHost.text, Number(mpdPort.text), mpdPassword.text)
    }
}
