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
import "../widgets"
import ".." // import Style.qml

import "MpdSongListView.js" as JS

TddListView {
    id: root
    property bool multiSelect: false // cannot be changed!

    property int selectedIndex: -1
    property string selectedPath: ""
    property int numSelected: 0

    onCountChanged: JS.updateSelectedIndices()

    function getSelectedPaths() {
        if (!multiSelect)
            return [selectedPath]
        var arr = []
        for (var i=0; i<count; i++)
            if (JS.selectedIndices[i])
                arr.push(JS.selectedIndices[i])
        return arr
    }

    function selectAll() { if (multiSelect) JS.selectAll() }
    function deselectAll() { if (multiSelect) JS.deselectAll() }

    signal selectionChanged()
    onSelectionChanged: numSelected = multiSelect?JS.getNumSelected():(selectedIndex!=-1);

    delegate: Rectangle {
        width: parent.width
        height: Style.rowHeight/2
        // color: steelblue if selected, else alternating row colors
        color: selected ? "steelblue" : (index%2?"white":"#ddd")
        property bool selected: multiSelect?JS.isSelected(index):index===selectedIndex
        Connections {
            target: root
            onSelectionChanged: if (multiSelect) selected = JS.isSelected(index)
        }
        Text {
            text: description
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            anchors { fill: parent; margins: 3; }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (multiSelect) {
                    JS.toggleSelect(index, path)
                } else {
                    selectedIndex = index
                    selectedPath = path
                    selectionChanged()
                }
            }
        }
    }
}
