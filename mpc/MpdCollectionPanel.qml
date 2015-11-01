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

import "mpd.js" as MPD

Rectangle {
    width: 4*Style.first8ColumnsWidth+4*Style.last4ColumnsWidth
    height: 7*Style.rowHeight
    color: "black"

    property int numSelectedSongs: mpdConnector.collectionModel.numSelectedSongs
    property int numSelectedDirs: mpdConnector.collectionModel.numSelectedDirectories
    property bool hasSelection: numSelectedDirs+numSelectedSongs > 0

    MpdButton {
        anchors.right: parent.right
        width: Style.last4ColumnsWidth
        image: "images/folder-up.png"
        onClicked: {
            var path = mpdConnector.currentCollectionPath
            path = path.substring(0, path.lastIndexOf("/"))
            mpdConnector.listDirectory(path)
        }
    }

    Column {
        width: parent.width
        MpdText {
            width: parent.width-Style.last4ColumnsWidth
            height: Style.rowHeight
            text: "Contents of:\n/"+mpdConnector.currentCollectionPath
            textItem.wrapMode: Text.WordWrap
        }
        Rectangle {
            width: parent.width
            height: 5*Style.rowHeight
            color: "white"
            TddListView {
                anchors.fill: parent
                model: mpdConnector.collectionModel
                delegate: Rectangle {
                    width: parent.width
                    height: Style.rowHeight/2
                    // color: steelblue if selected, else alternating row colors
                    color: selected ? "steelblue" : (index%2?"white":"#ddd")
                    property bool selected: mpdConnector.collectionModel.isSelected(index)
                    Connections {
                        target:mpdConnector.collectionModel
                        onSelectionChanged: selected = mpdConnector.collectionModel.isSelected(index)
                    }
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        x: 3
                        source: type=="Directory"?"images/folder.png":"images/song.png"
                    }
                    Text {
                        text: description
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        anchors { fill: parent; margins: 3; leftMargin: 22 }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: mpdConnector.collectionModel.toggleSelection(index)
                        onDoubleClicked: if (type=="Directory") mpdConnector.listDirectory(path)
                    }
                }
            }
        }
        Row {
            MpdButton {
                width: 4*Style.first8ColumnsWidth
                text: "Add selected "+numSelectedSongs+" songs and "+numSelectedDirs+" directories"
                image: "images/append.png"
                enabled: hasSelection
                onClicked: mpdConnector.addSongs(mpdConnector.collectionModel.getSelectedPaths())
            }
            MpdButton {
                width: 2*Style.last4ColumnsWidth
                text: "Select all"
                onClicked: mpdConnector.collectionModel.selectAll()
            }
            MpdButton {
                width: 2*Style.last4ColumnsWidth
                text: "Select none"
                onClicked: mpdConnector.collectionModel.deselectAll()
            }
        }
    }

    Component.onCompleted: mpdConnector.listDirectory("")
}
