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

import ".." // include Style.qml
import "../widgets"

Item {
    width: Style.first8ColumnsWidth
    height: Style.rowHeight

    id: button

    property alias text: label.text
    property alias textColor: label.color
    property alias borderColor: outer.color
    property alias color: inner.color
    property alias font: label.font
    property alias image: image.source

    signal clicked
    signal pressAndHold

    property int borderWidth: 2
    property int radius: 5
    property bool active: false

    Rectangle{
        id: outer
        width: button.width
        height: button.height
        radius: button.radius
        color: active?Style.mpdActiveButtonBorderColor:Style.mpdButtonBorderColor

        anchors.centerIn: parent

        Rectangle{
            id: inner
            width: button.width-2*button.borderWidth
            height: button.height-2*button.borderWidth
            radius: Math.max(0, button.radius - borderWidth)
            color: Style.mpdButtonColor

            anchors.centerIn: parent

            Image {
                id: image
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: label.text?undefined:parent.horizontalCenter
                anchors.left: label.text?parent.left:undefined
                anchors.margins: 5
                opacity: enabled?1:0.3
            }

            Text {
                id: label;
                text: "";
                color: enabled?Style.mpdTextColor:Style.mpdDisabledTextColor
                horizontalAlignment: image.width>0?Text.AlignLeft:Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
                anchors.left: image.width>0?image.right:parent.left
                anchors.leftMargin: image.width>0?5:0
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: button.clicked()
        onPressAndHold: button.pressAndHold()
    }
}
