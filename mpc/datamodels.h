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

#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QAbstractListModel>
#include <QStringList>
#include "types.h"

class MpdEntityListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MpdEntityListModel(QObject *parent);

    virtual void setEntityList(MpdEntityList list);

    virtual QHash<int, QByteArray> roleNames() const { return p_roles; }
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

protected:
    MpdEntityList m_list;
    QHash<int, QByteArray> p_roles;
};

class MpdCollectionModel : public MpdEntityListModel
{
    Q_OBJECT
    // status properties
    Q_PROPERTY( int numSelectedSongs READ getNumSelectedSongs NOTIFY selectionChanged )
    Q_PROPERTY( int numSelectedDirectories READ getNumSelectedDirectories NOTIFY selectionChanged )
public:
    MpdCollectionModel(QObject *parent);

    virtual void setEntityList(MpdEntityList list);

    Q_INVOKABLE bool isSelected(int index) const;
    int getNumSelectedSongs() const;
    int getNumSelectedDirectories() const;
    Q_INVOKABLE QStringList getSelectedPaths() const;

    virtual QVariant data(const QModelIndex &index, int role) const;

signals:
    void selectionChanged();

public slots:
    void toggleSelection(int index);
    void selectAll();
    void deselectAll();

private:
    QList<bool> m_selectedIndices;
};

class MpdSongListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MpdSongListModel(QObject *parent);

    void setSongList(MpdSongList list);
    Q_INVOKABLE bool containsId(int songId) { return !m_list.getSongById(songId).isNull(); }
    Q_INVOKABLE int getSongIndex(int songId) { return m_list.getSongIndex(songId); }
    Q_INVOKABLE QVariant getData(int index, QString roleName) const;

    virtual QHash<int, QByteArray> roleNames() const { return p_roles; }
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private:
    MpdSongList m_list;
    QHash<int, QByteArray> p_roles;
};

#endif // DATAMODELS_H
