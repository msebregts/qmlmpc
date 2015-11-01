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
import "../widgets"
import "mpd.js" as MPD

Rectangle {
    width: 4*Style.first8ColumnsWidth+4*Style.last4ColumnsWidth
    height: 7*Style.rowHeight
    color: "black"

    property string selectedPlaylist
    property alias selectedPath: playlistSongsView.selectedPath

    onVisibleChanged: if (visible) { mpdConnector.renewPlaylists(); selectedPlaylist = ""; selectedPath = "" }
    onSelectedPlaylistChanged: mpdConnector.getPlaylistSongs(selectedPlaylist)

    Row{
        Column {
            width: 4*Style.first8ColumnsWidth
            MpdText {
                width: parent.width
                height: Style.rowHeight/2
                text: "Playlists"
            }
            Rectangle {
                width: parent.width
                height: 4.5*Style.rowHeight
                TddListView {
                    id: playlistsView
                    anchors.fill: parent
                    model: mpdConnector.playlistsModel
                    delegate: Rectangle {
                        width: parent.width
                        height: Style.rowHeight/2
                        // color: steelblue if selected, else alternating row colors
                        color: description===selectedPlaylist ? "steelblue" : (index%2?"white":"#ddd")
                        Text {
                            text: description
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                            anchors { fill: parent; margins: 3; }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: selectedPlaylist = description
                        }
                    }
                }
            }
            Row {
                MpdButton {
                    width: 2*Style.first8ColumnsWidth
                    text: "Play now"
                    image: "images/play-small.png"
                    enabled: selectedPlaylist != ""
                    onClicked: mpdConnector.playPlaylist(selectedPlaylist)
                }
                MpdButton {
                    width: 2*Style.first8ColumnsWidth
                    text: "Add to current playlist"
                    image: "images/append.png"
                    enabled: selectedPlaylist != ""
                    onClicked: mpdConnector.appendPlaylist(selectedPlaylist)
                }
            }
            Row {
                MpdButton {
                    id: renameButton
                    width: 2*Style.first8ColumnsWidth
                    text: "Rename"
                    image: "images/rename.png"
                    enabled: selectedPlaylist != ""
                    onClicked: onConfirmed("")
                    function onConfirmed(text) { // called when the prompt --v is confirmed
                        if (text=="")
                            return prompt("Enter a new name for the playlist \""+selectedPlaylist+"\":", renameButton)
                        mpdConnector.renamePlaylist(selectedPlaylist, text)
                        mpdConnector.renewPlaylists()
                        selectedPlaylist = text // keep the playlist selected, but under the new name
                    }
                }
                MpdButton {
                    id: deleteButton
                    width: 2*Style.first8ColumnsWidth
                    text: "Delete"
                    image: "images/remove.png"
                    enabled: selectedPlaylist != ""
                    onClicked: confirm("Are you sure you want to delete the playlist \""+selectedPlaylist+"\"?", deleteButton)
                    function onConfirmed() { // called when the confirm dialog --^ is confirmed
                        mpdConnector.removePlaylist(selectedPlaylist)
                        mpdConnector.renewPlaylists()
                        selectedPlaylist = ""
                    }
                }
            }
        }
        Column {
            width: 4*Style.last4ColumnsWidth
            MpdText {
                text: "Songs in this playlist"
                width: parent.width
                height: Style.rowHeight/2
            }
            Rectangle {
                width: parent.width
                height: 5.5*Style.rowHeight
                MpdSongListView {
                    id: playlistSongsView
                    anchors.fill: parent
                    model: mpdConnector.playlistSongsModel
                    onSelectedPathChanged: if (selectedPath=="") selectedIndex = -1
                }
            }

            Row {
                MpdButton {
                    image: "images/insert.png"
                    text: "Add after current song"
                    width: 2*Style.last4ColumnsWidth
                    enabled: selectedPath != ""
                    onClicked: mpdConnector.insertSong(selectedPath)
                }
                MpdButton {
                    image: "images/append.png"
                    text: "Add to end of playlist"
                    width: 2*Style.last4ColumnsWidth
                    enabled: selectedPath != ""
                    onClicked: mpdConnector.appendSong(selectedPath)
                }
            }
        }
    }
}
