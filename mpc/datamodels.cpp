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

#include "datamodels.h"

MpdEntityListModel::MpdEntityListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p_roles[Qt::UserRole] = "description";
    p_roles[Qt::UserRole+1] = "type";
}

void MpdEntityListModel::setEntityList(MpdEntityList list)
{
    emit beginResetModel();
    m_list = list;
    emit endResetModel();
}

int MpdEntityListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()?0:m_list.length();
}

QVariant MpdEntityListModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_list.length())
        return QVariant();
    switch(role) {
    case Qt::UserRole:
        return m_list.at(index.row())->getDescription();
    case Qt::UserRole+1:
        MpdEntity::Type type = m_list.at(index.row())->getType();
        switch(type) {
        case MpdEntity::Directory:
            return "Directory";
        case MpdEntity::Song:
            return "Song";
        case MpdEntity::Playlist:
            return "Playlist";
        }
    }
    return QVariant();
}

MpdSongListModel::MpdSongListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p_roles[Qt::UserRole] = "description";
    p_roles[Qt::UserRole+1] = "path";
    p_roles[Qt::UserRole+2] = "title";
    p_roles[Qt::UserRole+3] = "artist";
    p_roles[Qt::UserRole+4] = "album";
    p_roles[Qt::UserRole+5] = "duration";
    p_roles[Qt::UserRole+6] = "songId";
    p_roles[Qt::UserRole+7] = "songPos";
}

void MpdSongListModel::setSongList(MpdSongList list)
{
    emit beginResetModel();
    m_list = list;
    emit endResetModel();
}

QVariant MpdSongListModel::getData(int index, QString roleName) const
{
    int role = roleNames().key(roleName.toUtf8(), -1);
    if (role == -1)
        return QVariant();
    return data(createIndex(index, 0), role);
}

int MpdSongListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()?0:m_list.length();
}

QVariant MpdSongListModel::data(const QModelIndex &index, int role) const
{
    if (index.row()<0 || index.row()>=m_list.length())
        return QVariant();
    switch(role) {
    case Qt::UserRole:
        return m_list.at(index.row())->getDescription();
    case Qt::UserRole+1:
        return m_list.at(index.row())->getPath();
    case Qt::UserRole+2:
        return m_list.at(index.row())->getTitle();
    case Qt::UserRole+3:
        return m_list.at(index.row())->getArtist();
    case Qt::UserRole+4:
        return m_list.at(index.row())->getAlbum();
    case Qt::UserRole+5:
        return m_list.at(index.row())->getDuration();
    case Qt::UserRole+6:
        return m_list.at(index.row())->getPlaylistId();
    case Qt::UserRole+7:
        return m_list.at(index.row())->getPlaylistPosition();
    }
    return QVariant();
}




MpdCollectionModel::MpdCollectionModel(QObject *parent) :
    MpdEntityListModel(parent)
{
    p_roles[Qt::UserRole] = "description";
    p_roles[Qt::UserRole+1] = "type";
    p_roles[Qt::UserRole+2] = "path";
}

void MpdCollectionModel::setEntityList(MpdEntityList list)
{
    emit beginResetModel();
    m_list = MpdEntityList();
    m_selectedIndices.clear();
    for (int i=0; i<list.length(); i++) {
        MpdEntity::Type t = list.at(i)->getType();
        if (t == MpdEntity::Directory || t == MpdEntity::Song) {
            m_list.append(list.at(i));
            m_selectedIndices.append(false);
        }
    }
    emit endResetModel();
    emit selectionChanged();
}

bool MpdCollectionModel::isSelected(int index) const
{
    return m_selectedIndices[index];
}

int MpdCollectionModel::getNumSelectedSongs() const
{
    int num = 0;
    for (int i=0; i<m_selectedIndices.length(); i++)
        if (m_selectedIndices[i] && m_list.at(i)->getType()==MpdEntity::Song)
            num ++;
    return num;
}

int MpdCollectionModel::getNumSelectedDirectories() const
{
    int num = 0;
    for (int i=0; i<m_selectedIndices.length(); i++)
        if (m_selectedIndices[i] && m_list.at(i)->getType()==MpdEntity::Directory)
            num ++;
    return num;
}

QStringList MpdCollectionModel::getSelectedPaths() const
{
    QStringList paths;
    for (int i=0; i<m_selectedIndices.length(); i++)
        if (m_selectedIndices[i])
            paths.append(m_list.at(i)->getPath());
    return paths;
}

QVariant MpdCollectionModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::UserRole+2) {
        return m_list.at(index.row())->getPath();
    }
    return MpdEntityListModel::data(index, role);
}

void MpdCollectionModel::toggleSelection(int index)
{
    m_selectedIndices[index] = !m_selectedIndices[index];
    emit selectionChanged();
}

void MpdCollectionModel::selectAll()
{
    for (int i=0; i<m_selectedIndices.length(); i++)
        m_selectedIndices[i] = true;
    emit selectionChanged();
}

void MpdCollectionModel::deselectAll()
{
    for (int i=0; i<m_selectedIndices.length(); i++)
        m_selectedIndices[i] = false;
    emit selectionChanged();
}
