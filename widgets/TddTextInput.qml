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

Rectangle {
    property alias textInput: ti
    property alias text: ti.text

    color: ti.focus ? "blue" : "black"
    radius: 2
    height: ti.font.pixelSize*1.4+2

    function forceActiveFocus() { ti.forceActiveFocus() }

    Rectangle {
        color: "white"
        radius: 1
        anchors { fill: parent; margins: 1 }

        TextInput {
            id: ti

            selectByMouse: true
            color: "black"
            selectionColor: "blue"

            anchors { fill: parent; margins: 1 }

            Keys.onReturnPressed: Qt.inputMethod.hide()
        }
    }
}
