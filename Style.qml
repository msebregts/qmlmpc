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

pragma Singleton

import QtQuick 2.0

QtObject {
    // grid
    property real rowHeight: 86
    property real topRowHeight: 87
    property real first8ColumnsWidth: 106
    property real last4ColumnsWidth: 108

    property real totalWidth: 1280
    property real totalHeight: 800-25 // 25 pixels reserved for android notification bar topRowHeight+8*rowHeight

    // styles
    property color mpdButtonColor: "#444"
    property color mpdButtonBorderColor: "black"
    property color mpdTextColor: "white"
    property color mpdDisabledTextColor: "#888"
    property color mpdActiveButtonBorderColor: "white"
}
