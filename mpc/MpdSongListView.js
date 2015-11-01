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

var selectedIndices = []

function updateSelectedIndices() {
    if (multiSelect) {
        selectedIndices = new Array()
        selectedIndices[count] = 1
    } else {
        selectedIndices = undefined
    }
    selectionChanged()
}

function toggleSelect(index, path) {
    if (selectedIndices[index])
        selectedIndices[index] = undefined
    else
        selectedIndices[index] = path
    selectionChanged()
}

function getNumSelected() {
    var num = 0
    for (var i=0; i<count; i++)
        num += isSelected(i)
    return num
}

function isSelected(index) {
    return Boolean(selectedIndices[index])
}

function selectAll() {
    for (var i=0; i<count; i++)
        selectedIndices[i] = model.getData(i, "path");
    selectionChanged()
}

function deselectAll() {
    for (var i=0; i<count; i++)
        selectedIndices[i] = undefined
    selectionChanged()
}
