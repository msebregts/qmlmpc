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

import "mpd.js" as MPD
import "../widgets"
import ".." // import Style.qml

Rectangle {
    width: Style.totalWidth
    height: 8*Style.rowHeight
    color: "black"

    property int activePanel: MPD.overviewPanel

    Rectangle {
        anchors.fill: parent
        visible: !mpdConnector.connection.connected
        color: "black"
        Text {
            anchors.centerIn: parent
            text: "The connection to the server is lost.\n"+
                  "Click anywhere to try to restore the connection, or change your settings."
            color: "white"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: mpdConnector.connection.reconnect()
        }
        z: 1000
    }

    Column {
        Row {
            MpdButton {
                id: previous
                image: "images/prev.png"
                onClicked: mpdConnector.previous()
            }
            MpdButton {
                id: play
                visible: !mpdConnector.playing
                image: "images/play.png"
                onClicked: mpdConnector.play()
            }
            MpdButton {
                id: pause
                visible: mpdConnector.playing
                image: "images/pause.png"
                onClicked: mpdConnector.pause()
            }
            MpdButton {
                id: stop
                image: "images/stop.png"
                onClicked: mpdConnector.stop()
            }
            MpdButton {
                id: next
                image: "images/next.png"
                onClicked: mpdConnector.next()
            }
            MpdButton {
                id: repeat
                active: mpdConnector.repeating
                image: "images/repeat.png"
                onClicked: mpdConnector.repeat(!active)
            }
            MpdButton {
                id: shuffle
                active: mpdConnector.shuffling
                image: "images/shuffle.png"
                onClicked: mpdConnector.random(!active)
            }
            MpdButton {
                id: playlistsButton
                text: "Playlists"
                width: 2*Style.first8ColumnsWidth
                active: activePanel === MPD.playlistsPanel
                onClicked: MPD.setActivePanel(MPD.playlistsPanel)
            }
            MpdButton {
                id: collectionButton
                text: "Collection"
                width: 2*Style.last4ColumnsWidth
                active: activePanel === MPD.collectionPanel
                onClicked: MPD.setActivePanel(MPD.collectionPanel)
            }
            MpdButton {
                id: searchButton
                text: "Search"
                width: 2*Style.last4ColumnsWidth
                active: activePanel === MPD.searchPanel
                onClicked: MPD.setActivePanel(MPD.searchPanel)
            }
        }


        Row {
            Item {
                width: 4*Style.first8ColumnsWidth
                height: playlistColumn.height

                Column {
                id: playlistColumn
                width: parent.width
                Item {
                    id: progressBar
                    width: parent.width
                    height: Style.rowHeight/2
                    Rectangle {
                        anchors { fill: parent; margins: 1 }
                        color: "#999"
                        radius: 4
                        clip: true
                        Rectangle {
                            anchors { left: parent.left; top: parent.top; bottom: parent.bottom}
                            radius: Math.min(4, width/2)
                            color: "black"
                            width: mpdConnector.elapsedTime/mpdConnector.totalTime*parent.width
                            Rectangle {
                                x: 1
                                y: 1
                                width: Math.max(0, parent.width-2)
                                height: parent.height-2
                                color: "#444"
                                radius: Math.max(0, parent.radius-1)
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: mpdConnector.seek(mouseX*mpdConnector.totalTime/parent.width)
                        }
                    }
                    Text {
                        anchors.centerIn: parent
                        text: MPD.formatSeconds(mpdConnector.elapsedTime)+" / "+MPD.formatSeconds(mpdConnector.totalTime)
                        color: "white"
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 5.5*Style.rowHeight
                    TddListView {
                        id: queueView
                        anchors.fill: parent
                        model: mpdConnector.queueModel
                        property int savedScroll
                        onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Visible)
                        Connections {
                            target: queueView.model
                            onModelAboutToBeReset: queueView.savedScroll = queueView.contentY
                            onModelReset: {
                                queueView.contentY = queueView.savedScroll
                                var i = queueView.model.getSongIndex(overviewPanel.selectedSongId)
                                if (i!==-1) queueView.currentIndex = i
                            }
                        }
                        delegate: Rectangle {
                            width: parent.width
                            height: Style.rowHeight/2
                            // color: steelblue if selected, else alternating row colors
                            color: songId===overviewPanel.selectedSongId ? "steelblue" : (index%2?"white":"#ddd")
                            Image {
                                id: img
                                source: "images/play-mini.png"
                                visible: songId===mpdConnector.currentSongId
                                anchors { left: parent.left; leftMargin: 3; verticalCenter: parent.verticalCenter }
                            }
                            Text {
                                text: description
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                                anchors { fill: parent; margins: 3; }
                                anchors.leftMargin: img.visible?img.width+6:3
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    overviewPanel.selectedSongId = songId
                                    overviewPanel.selectedSongTitle = title
                                    overviewPanel.selectedSongAlbum = album
                                    overviewPanel.selectedSongArtist = artist
                                    overviewPanel.selectedSongDuration = duration
                                    queueView.currentIndex = index
                                }
                                onDoubleClicked: mpdConnector.playSong(songId)
                            }
                        }
                    }
                }

                Row {
                    MpdButton {
                        width: 2*Style.first8ColumnsWidth
                        text: "Scroll to current song"
                        onClicked: queueView.positionViewAtIndex(mpdConnector.currentSongPos, ListView.Visible)
                    }
                    MpdButton {
                        width: 2*Style.first8ColumnsWidth
                        text: "Scroll to selected song"
                        onClicked: queueView.positionViewAtIndex(queueView.currentIndex, ListView.Visible)
                    }
                }
            }

                Rectangle {
                    anchors.fill: playlistColumn
                    color: "#88000000"
                    visible: activePanel !== MPD.overviewPanel
                    MouseArea {
                        anchors.fill: parent
                        onClicked: activePanel = MPD.overviewPanel
                    }
                }
            }
            MpdOverviewPanel {
                id: overviewPanel
                visible: activePanel === MPD.overviewPanel
            }
            MpdPlaylistsPanel {
                id: playlistsPanel
                visible: activePanel === MPD.playlistsPanel
            }
            MpdCollectionPanel {
                id: collectionPanel
                visible: activePanel === MPD.collectionPanel
            }
            MpdSearchPanel {
                id: searchPanel
                visible: activePanel === MPD.searchPanel
            }
        }
    }

    // messagebox things:
    function confirm(text, obj) {
        messageLabel.text = text
        messagePrompt.visible = false
        messagebox.listeningObject = obj
        messagebox.visible = true
    }
    function prompt(text, obj) {
        messageLabel.text = text
        messagePrompt.visible = true
        messagePrompt.text = ""
        messagebox.listeningObject = obj
        messagebox.visible = true
        messagePrompt.forceActiveFocus()
    }

    Rectangle {
        id: messagebox

        anchors.fill: parent
        color: "black"
        visible: false

        property variant listeningObject

        function confirmed(text) {
            if (listeningObject.onConfirmed) {
                if (messagePrompt.visible) {
                    visible = false
                    listeningObject.onConfirmed(text)
                } else {
                    visible = false
                    listeningObject.onConfirmed()
                }
            }
        }

        function canceled() {
            visible = false
            if (listeningObject.onCanceled)
                listeningObject.onCanceled()
        }

        Column {
            anchors.centerIn: parent
            spacing: 5
            Text {
                id: messageLabel
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white";
                width: parent.parent.width*0.75
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            TddTextInput {
                id: messagePrompt
                width: messageLabel.width
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 50
                MpdButton {
                    text: "OK"
                    color: "#080"
                    width: 3*Style.first8ColumnsWidth
                    onClicked: messagebox.confirmed(messagePrompt.text)
                }
                MpdButton {
                    text: "Cancel"
                    color: "#800"
                    width: 3*Style.first8ColumnsWidth
                    onClicked: messagebox.canceled()
                }
            }
        }
    }
}
