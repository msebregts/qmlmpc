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

#include "mpdconnector.h"

#include "musicplayerconnection.h"
#include "datamodels.h"

#define SIMPLE_REQUEST(func) MpdRequest *req = p_connection->func; if (req) connect(req, SIGNAL(resultReady()), p_connection, SLOT(debugAndDelete()));
#define MOVE_REQUEST(songId, direction) MpdSong *song = getSong(songId); if (song) { SIMPLE_REQUEST(move(song, direction)) } else { qDebug("Move: invalid songId"); }

MpdConnector::MpdConnector(QString host, int port, QString password, QObject *parent) :
    QObject(parent)
{
    p_connection = new MusicPlayerConnection(host, port, password, this);
    connect(p_connection, SIGNAL(statusChanged(QSharedPointer<MpdStatus>)), SLOT(statusUpdated(QSharedPointer<MpdStatus>)));

    p_status = QSharedPointer<MpdStatus>(new MpdStatus("")); //construct default status
    m_lastPlaylistVersion = -1; // force update when next status is ready
    p_queueModel = new MpdSongListModel(this);
    p_playlistsModel = new MpdEntityListModel(this);
    p_playlistSongsModel = new MpdSongListModel(this);
    p_collectionModel = new MpdCollectionModel(this);
    p_searchResultModel = new MpdSongListModel(this);
}

QString MpdConnector::getNowPlaying()
{
    MpdSong *song = getSong(p_status->currentSongId());
    if (song)
        return song->getDescription();
    return "";
}

void MpdConnector::play() { SIMPLE_REQUEST(play()) }
void MpdConnector::pause() { SIMPLE_REQUEST(pause()) }
void MpdConnector::stop() { SIMPLE_REQUEST(stop()) }
void MpdConnector::next() { SIMPLE_REQUEST(next()) }
void MpdConnector::previous() { SIMPLE_REQUEST(previous()) }
void MpdConnector::repeat(bool repeat) { SIMPLE_REQUEST(repeat(repeat)) }
void MpdConnector::random(bool random) { SIMPLE_REQUEST(random(random)) }
void MpdConnector::playSong(int songId) { SIMPLE_REQUEST(playSong(songId)) }
void MpdConnector::removeSong(int songId) { SIMPLE_REQUEST(removeSong(songId)) }
void MpdConnector::clearQueue() { SIMPLE_REQUEST(clearQueue()) }
void MpdConnector::savePlaylist(QString name) { SIMPLE_REQUEST(savePlaylist(name)) }
void MpdConnector::renamePlaylist(QString playlist, QString newName) { SIMPLE_REQUEST(renamePlaylist(playlist, newName)) }
void MpdConnector::removePlaylist(QString playlist) { SIMPLE_REQUEST(removePlaylist(playlist)) }
void MpdConnector::appendPlaylist(QString playlist) { SIMPLE_REQUEST(appendPlaylist(playlist)) }
void MpdConnector::playPlaylist(QString playlist) { SIMPLE_REQUEST(playPlaylist(playlist)) }
void MpdConnector::insertSong(QString path) { if (getCurrentSongId()==-1) { return qDebug("Cannot insert song: no song is playing."); } SIMPLE_REQUEST(insertSong(path)) }
void MpdConnector::appendSong(QString path) { SIMPLE_REQUEST(appendSong(path)) }
void MpdConnector::prependSong(QString path) { SIMPLE_REQUEST(prependSong(path)) }
void MpdConnector::addSongs(QStringList paths) { SIMPLE_REQUEST(addSongs(paths)) }
void MpdConnector::moveSongUp(int songId) { MOVE_REQUEST(songId, MusicPlayerConnection::MoveOneUp) }
void MpdConnector::moveSongDown(int songId) { MOVE_REQUEST(songId, MusicPlayerConnection::MoveOneDown) }
void MpdConnector::moveSongFirst(int songId) { MOVE_REQUEST(songId, MusicPlayerConnection::MoveToFirst) }
void MpdConnector::moveSongLast(int songId) { MOVE_REQUEST(songId, MusicPlayerConnection::MoveToLast) }
void MpdConnector::moveSongAfterCurrent(int songId) { MOVE_REQUEST(songId, MusicPlayerConnection::MoveAfterCurrent) }

void MpdConnector::seek(int time)
{
    SIMPLE_REQUEST(seek(p_status->currentSongId(), time));
}

void MpdConnector::listDirectory(QString path)
{
    m_currentCollectionPath = path;
    emit currentCollectionPathChanged();
    p_collectionModel->setEntityList(MpdEntityList());
    MpdEntityListRequest *request = p_connection->listDirectory(path);
    connect(request, SIGNAL(resultReady()), SLOT(directoryListingReady()));
}

void MpdConnector::directoryListingReady()
{
    MpdEntityListRequest *request = qobject_cast<MpdEntityListRequest*>(sender());
    if (request->succesfull()) {
        p_collectionModel->setEntityList(request->getEntityList());
    } else {
        qDebug("listDirectory got an ACK: '%s'", qPrintable(request->getAck()));
    }
    request->deleteLater();
}

void MpdConnector::renewPlaylists()
{
    MpdEntityListRequest *request = p_connection->listPlaylists();
    connect(request, SIGNAL(resultReady()), SLOT(playlistsReady()));
}

void MpdConnector::playlistsReady()
{
    MpdEntityListRequest *request = qobject_cast<MpdEntityListRequest*>(sender());
    if (request->succesfull()) {
        p_playlistsModel->setEntityList(request->getEntityList());
    } else {
        qDebug("listPlaylists got an ACK: '%s'", qPrintable(request->getAck()));
    }
    request->deleteLater();
}

void MpdConnector::getPlaylistSongs(QString playlist)
{
    if (playlist.isEmpty())
        return p_playlistSongsModel->setSongList(MpdSongList());
    MpdSongListRequest *request = p_connection->getPlaylistSongs(playlist);
    connect(request, SIGNAL(resultReady()), SLOT(playlistSongsReady()));
}

void MpdConnector::playlistSongsReady()
{
    MpdSongListRequest *request = qobject_cast<MpdSongListRequest*>(sender());
    if (request->succesfull()) {
        p_playlistSongsModel->setSongList(request->getSongList());
    } else {
        qDebug("listPlaylistSongs got an ACK: '%s'", qPrintable(request->getAck()));
    }
    request->deleteLater();
}

void MpdConnector::search(QString query, QString scope)
{
    MpdSongListRequest *request = p_connection->search(query, scope);
    connect(request, SIGNAL(resultReady()), SLOT(searchResultsReady()));
}

void MpdConnector::searchResultsReady()
{
    MpdSongListRequest *request = qobject_cast<MpdSongListRequest*>(sender());
    if (request->succesfull()) {
        p_searchResultModel->setSongList(request->getSongList());
    } else {
        qDebug("search got an ACK: '%s'", qPrintable(request->getAck()));
    }
    request->deleteLater();
}

void MpdConnector::statusUpdated(QSharedPointer<MpdStatus> status)
{
    p_status = status;
    emit statusChanged();
    emit currentSongChanged();
    if (status->playlistVersion()!=m_lastPlaylistVersion) {
        // request the queue
        MpdSongListRequest *req = p_connection->getQueue();
        connect(req, SIGNAL(resultReady()), SLOT(queueReady()));
        m_lastPlaylistVersion = status->playlistVersion();
    }
}

void MpdConnector::queueReady()
{
    MpdSongListRequest *req = qobject_cast<MpdSongListRequest*>(sender());
    if (!req || !req->succesfull()) {
        qDebug("getQueue() unsuccesfull");
        m_lastPlaylistVersion = -1;
        statusUpdated(p_status); // force updating the queue
    } else { // success :)
        m_queue = req->getSongList();
        p_queueModel->setSongList(m_queue);
    }
    if (req)
        req->deleteLater();
}

MpdSong *MpdConnector::getSong(int songId)
{
    if (songId==-1)
        return 0;
    return m_queue.getSongById(songId).data();
}
