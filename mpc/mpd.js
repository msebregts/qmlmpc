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

var overviewPanel = 1
var playlistsPanel = 2
var collectionPanel = 3
var searchPanel = 4

function setActivePanel(panel) {
    if (activePanel === panel)
        activePanel = overviewPanel
    else
        activePanel = panel
}

function formatSeconds(time) {
    var seconds = new String(time%60)
    return Math.floor(time/60)+":"+(seconds.length==1?"0"+seconds:seconds)
}
