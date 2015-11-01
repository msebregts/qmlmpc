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

import ".." // import Style.qml
import "mpd.js" as MPD

Rectangle {
    width: 4*Style.first8ColumnsWidth+4*Style.last4ColumnsWidth
    height: 7*Style.rowHeight
    color: "black"

    property string searchText
    property string searchScope: "any" // any, artist, title (http://mpd.wikia.com/wiki/MusicPlayerDaemonCommands#Scope_specifiers)
    property string searchScopeText: "Any" // what the button should say

    function doSearch() {
        if (searchText)
            mpdConnector.search(searchText, searchScope)
    }
    onSearchTextChanged: doSearch()
    onSearchScopeChanged: doSearch()

    Column {
        anchors.fill: parent
        Item {
            height: Style.rowHeight/2
            width: parent.width
            MpdText {
                id: headerText
                text: "Search:"
                height: parent.height
                width: Style.rowHeight
            }
            Rectangle {
                id: searchInputField
                height: 24
                anchors { left: headerText.right; right: searchOptions.left; verticalCenter: parent.verticalCenter }
                radius: 3
                color: "white"
                Text {
                    anchors { fill: parent; margins: 2 }
                    text: searchText
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: prompt("Enter search query:", searchInputField)
                }
                function onConfirmed(text) { searchText = text }
            }
            MpdButton {
                id: searchOptions
                width: 2*Style.last4ColumnsWidth
                height: Style.rowHeight/2
                anchors.right: parent.right
                image: "images/drop-down.png"
                text: searchScopeText
                onClicked: active = !active
                Column {
                    id: searchScopeDropdown

                    y: parent.height
                    visible: searchOptions.active

                    Repeater {
                        model: ["any:Any", "artist:Artist", "title:Title"]
                        delegate: Rectangle {
                            width: 2*Style.last4ColumnsWidth
                            height: Style.rowHeight/2
                            color: "#444"
                            border { width: 1; color: "black" }
                            property string scope: modelData.split(":")[0]
                            property string text: modelData.split(":")[1]
                            Text {
                                text: parent.text
                                color: "white"
                                anchors { left: parent.left; leftMargin: 26; verticalCenter: parent.verticalCenter }
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    searchScope = scope
                                    searchScopeText = text
                                    searchOptions.active = false // hide this dropdown
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            z: -1 // hide behind searchScopeDropdown
            width: parent.width
            height: parent.height-1.5*Style.rowHeight
            color: "white"
            MpdSongListView {
                id: songlistView
                anchors.fill: parent
                model: mpdConnector.searchResultModel
                multiSelect: true
            }
        }

        Row {
            MpdButton {
                width: 4*Style.first8ColumnsWidth
                text: "Append selected "+songlistView.numSelected+" songs"
                image: "images/append.png"
                enabled: songlistView.numSelected > 0
                onClicked: mpdConnector.addSongs(songlistView.getSelectedPaths())
            }
            MpdButton {
                width: 2*Style.last4ColumnsWidth
                text: "Select all"
                onClicked: songlistView.selectAll()
            }
            MpdButton {
                width: 2*Style.last4ColumnsWidth
                text: "Select none"
                onClicked: songlistView.deselectAll()
            }
        }
    }
}
