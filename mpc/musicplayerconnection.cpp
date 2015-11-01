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

#include "musicplayerconnection.h"

#include <QTcpSocket>
#include <QTimer>
#include <QStringList>

MusicPlayerConnection::MusicPlayerConnection(QString host, int port, QString password, QObject *parent) :
    QObject(parent),
    p_waitingRequest(0),
    m_connected(false)
{
    m_host = host;
    m_port = port;
    m_password = password;
    p_socket = new QTcpSocket(this);

    connect(p_socket, SIGNAL(readyRead()), this, SLOT(dataReady()));
    connect(p_socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(p_socket, SIGNAL(disconnected()), SLOT(onDisconnected()));
    connect(p_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onSocketError(QAbstractSocket::SocketError)));

    p_timer = new QTimer(this);
    connect(p_timer, SIGNAL(timeout()), this, SLOT(renewStatus()));
    p_timer->setInterval(100);

    QTimer *sendMessagesTimer = new QTimer(this);
    connect(sendMessagesTimer, SIGNAL(timeout()), SLOT(sendNextRequest()));
    sendMessagesTimer->setInterval(1000); // if a sendNextRequest had failed, it won't be resend without this timer
    sendMessagesTimer->start();

    p_socket->connectToHost(host, port);
}

MpdStatusRequest *MusicPlayerConnection::getStatus()
{
    MpdStatusRequest *request = new MpdStatusRequest("status");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::play()
{
    MpdRequest *request = new MpdRequest("play");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::pause()
{
    MpdRequest *request = new MpdRequest("pause");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::stop()
{
    MpdRequest *request = new MpdRequest("stop");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::next()
{
    MpdRequest *request = new MpdRequest("next");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::previous()
{
    MpdRequest *request = new MpdRequest("previous");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::repeat(bool repeat)
{
    MpdRequest *request = new MpdRequest(repeat?"repeat 1":"repeat 0");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::random(bool random)
{
    MpdRequest *request = new MpdRequest(random?"random 1":"random 0");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::move(MpdSong *song, MoveDirection direction)
{
    int pos = -1;
    switch(direction){
    case MoveToFirst:
        pos = 0;
        break;
    case MoveToLast:
        pos = p_status->playlistLength()-1;
        break;
    case MoveAfterCurrent:
        pos = p_status->currentSongPos();
        if (pos!=-1 && pos<song->getPlaylistPosition()) pos ++;
        break;
    case MoveOneUp:
        pos = qMax(0, song->getPlaylistPosition()-1);
        break;
    case MoveOneDown:
        pos = qMin(song->getPlaylistPosition()+1, p_status->playlistLength()-1);
        break;
    }
    if (pos<0)
        return 0;
    MpdRequest *request = new MpdRequest(QString("moveid \"%1\" \"%2\"").arg(song->getPlaylistId()).arg(pos));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::playSong(unsigned int songId)
{
    MpdRequest *request = new MpdRequest(QString("playid %1").arg(songId));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::removeSong(unsigned int songId)
{
    MpdRequest *request = new MpdRequest(QString("deleteid %1").arg(songId));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::clearQueue()
{
    MpdRequest *request = new MpdRequest("clear");
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::savePlaylist(QString name)
{
    MpdRequest *request = new MpdRequest(QString("save \"%1\"").arg(name));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::renamePlaylist(QString playlist, QString newName)
{
    MpdRequest *request = new MpdRequest(QString("rename \"%1\" \"%2\"").arg(playlist).arg(newName));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::removePlaylist(QString playlist)
{
    MpdRequest *request = new MpdRequest(QString("rm \"%1\"").arg(playlist));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::appendPlaylist(QString playlist)
{
    MpdRequest *request = new MpdRequest(QString("load \"%1\"").arg(playlist));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::playPlaylist(QString playlist)
{
    MpdRequest *request = new MpdRequest(QString("command_list_begin\nclear\nload \"%1\"\nplay\ncommand_list_end").arg(playlist));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::insertSong(QString path)
{
    MpdRequest *request = new MpdRequest(QString("addid \"%1\" -1").arg(path));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::appendSong(QString path)
{
    MpdRequest *request = new MpdRequest(QString("addid \"%1\"").arg(path));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::prependSong(QString path)
{
    MpdRequest *request = new MpdRequest(QString("addid \"%1\" 0").arg(path));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::addSongs(QStringList paths)
{
    MpdRequest *request;
    if (paths.isEmpty())
        request = new MpdRequest("ping");
    else
        request = new MpdRequest(QString("command_list_begin\nadd \"%1\"\ncommand_list_end").arg(paths.join("\"\nadd \"")));
    enqueueRequest(request);
    return request;
}

MpdRequest *MusicPlayerConnection::seek(int songId, int time)
{
    MpdRequest *request = new MpdRequest(QString("seekid \"%1\" \"%2\"").arg(songId).arg(time));
    enqueueRequest(request);
    return request;
}

MpdEntityListRequest *MusicPlayerConnection::listDirectory(QString path)
{
    MpdEntityListRequest *request = new MpdEntityListRequest(QString("lsinfo \"%1\"").arg(path));
    enqueueRequest(request);
    return request;
}

MpdEntityListRequest *MusicPlayerConnection::listPlaylists()
{
    MpdEntityListRequest *request = new MpdEntityListRequest("listplaylists");
    enqueueRequest(request);
    return request;
}

MpdSongListRequest *MusicPlayerConnection::getQueue()
{
    MpdSongListRequest *request = new MpdSongListRequest("playlistinfo");
    enqueueRequest(request);
    return request;
}

MpdSongListRequest *MusicPlayerConnection::getPlaylistSongs(QString playlist)
{
    MpdSongListRequest *request = new MpdSongListRequest(QString("listplaylistinfo \"%1\"").arg(playlist));
    enqueueRequest(request);
    return request;
}

MpdSongListRequest *MusicPlayerConnection::search(QString query, QString scope)
{
    MpdSongListRequest *request = new MpdSongListRequest(QString("search \"%1\" \"%2\"").arg(scope, query));
    enqueueRequest(request);
    return request;
}

void MusicPlayerConnection::debugAndDelete()
{
    MpdRequest *request = qobject_cast<MpdRequest*>(sender());
    if (!request) return; //ignore...
    if (!request->succesfull())
        qDebug("Got an ACK: %s", qPrintable(request->getAck()));
    request->deleteLater();
}

void MusicPlayerConnection::reconnect()
{
    if (!p_socket->isOpen())
        p_socket->connectToHost(m_host, m_port);
}

void MusicPlayerConnection::reconnect(QString host, int port, QString password)
{
    if (p_socket->isOpen())
        disconnect();
    m_host = host;
    m_port = port;
    m_password = password;
    p_socket->connectToHost(host, port);
}

void MusicPlayerConnection::disconnect()
{
    p_socket->abort();
}

void MusicPlayerConnection::dataReady()
{
    if (!p_socket->canReadLine())
        return;
    QByteArray data;
    if ( !m_connected ) {
        // this should be MPD's welcome message
        data = p_socket->readLine();
        if (data.startsWith("OK MPD")) {
            m_connected = true;
            emit connectedChanged();
            p_timer->start();
        } else {
            qDebug("Expected MPD welcome message, but got '%s'", data.constData());
            p_socket->disconnectFromHost();
            return;
        }
    }
    if (!p_waitingRequest) {
        qDebug("%s", p_socket->readAll().constData());
        qDebug("Got a message from the server, but no waiting request");
    }
    while (p_socket->canReadLine()) {
        data = p_socket->readLine();
        if (data.startsWith("OK")) {
            p_waitingRequest->setOk();
            p_waitingRequest = 0;
            sendNextRequest();
        } else if (data.startsWith("ACK")) {
            p_waitingRequest->setAck(QString::fromUtf8(data.constData()));
            p_waitingRequest = 0;
            sendNextRequest();
        } else {
            p_waitingRequest->feedData(data);
        }
    }
}

void MusicPlayerConnection::renewStatus()
{
    if (m_connected) {
        MpdStatusRequest *req = getStatus();
        connect(req, SIGNAL(resultReady()), SLOT(statusReady()));
    }
}

void MusicPlayerConnection::statusReady()
{
    MpdStatusRequest *req = qobject_cast<MpdStatusRequest*>(sender());
    if (p_status.data()==0 || *req->getStatus().data() != *p_status.data()) {
        p_status = req->getStatus();
        emit statusChanged(p_status);
    }
    req->deleteLater();
}

void MusicPlayerConnection::onSocketError(QAbstractSocket::SocketError error)
{
    qDebug("MPD Socket Error: %d", error);
    p_socket->close();
}

void MusicPlayerConnection::onConnected()
{
    qDebug("MPD connected");
    if (!m_password.isEmpty()) {
        MpdRequest *req = new MpdRequest(QString("password \"%1\"").arg(m_password));
        enqueueRequest(req);
        connect(req, SIGNAL(resultReady()), req, SLOT(deleteLater()));
    }
}

void MusicPlayerConnection::onDisconnected()
{
    qDebug("MPD disconnected");
    m_connected = false;
    emit connectedChanged();
    // discard waiting requests, they won't be answered...
    if (p_waitingRequest != 0)
        delete p_waitingRequest;
    p_waitingRequest = 0;
    qDeleteAll(m_requestQueue);
    m_requestQueue.clear();
}

void MusicPlayerConnection::enqueueRequest(MpdRequest *request)
{
    m_requestQueue.append(request);
    sendNextRequest();
}

void MusicPlayerConnection::sendNextRequest()
{
    if (p_waitingRequest!=0 || m_requestQueue.isEmpty())
        return;
    if (p_socket->state()!=QAbstractSocket::ConnectedState)
        return; // qDebug("MPC: Socket is not open");
    p_waitingRequest = m_requestQueue.takeFirst();
    p_socket->write(p_waitingRequest->m_requestMessage.toUtf8()+'\n');
    p_socket->flush();
}

