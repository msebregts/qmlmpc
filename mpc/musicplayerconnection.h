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

#ifndef MUSICPLAYERCONNECTION_H
#define MUSICPLAYERCONNECTION_H

#include <QObject>
#include <QAbstractSocket>
#include "types.h"

class QTcpSocket;
class QTimer;
class MpdRequest;
class MpdStatusRequest;
class MpdSongListRequest;
class MpdEntityListRequest;

class MusicPlayerConnection : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool connected READ isConnected NOTIFY connectedChanged )
public:
    enum MoveDirection {
        MoveToFirst,
        MoveToLast,
        MoveAfterCurrent,
        MoveOneUp,
        MoveOneDown
    };

    MusicPlayerConnection(QString host, int port, QString password=QString(), QObject *parent = 0);

    MpdStatusRequest *getStatus();
    MpdRequest *play();
    MpdRequest *pause();
    MpdRequest *stop();
    MpdRequest *next();
    MpdRequest *previous();
    MpdRequest *repeat(bool repeat);
    MpdRequest *random(bool random);
    MpdRequest *move(MpdSong *song, MoveDirection direction);
    MpdRequest *playSong(unsigned int songId);
    MpdRequest *removeSong(unsigned int songId);
    MpdRequest *clearQueue();
    MpdRequest *savePlaylist(QString name);
    MpdRequest *renamePlaylist(QString playlist, QString newName);
    MpdRequest *removePlaylist(QString playlist);
    MpdRequest *appendPlaylist(QString playlist);
    MpdRequest *playPlaylist(QString playlist);
    MpdRequest *insertSong(QString path);
    MpdRequest *appendSong(QString path);
    MpdRequest *prependSong(QString path);
    MpdRequest *addSongs(QStringList paths);
    MpdRequest *seek(int songId, int time);
    MpdEntityListRequest *listDirectory(QString path);
    MpdEntityListRequest *listPlaylists();
    MpdSongListRequest *getQueue();
    MpdSongListRequest *getPlaylistSongs(QString playlist);
    MpdSongListRequest *search(QString query, QString scope);

    bool isConnected() { return m_connected; }

signals:
    void statusChanged(QSharedPointer<MpdStatus> status);
    void connectedChanged();

public slots:
    void debugAndDelete(); //!< if an error occurred, display the error and finally delete the MpdRequest

    void reconnect();
    void reconnect(QString host, int port, QString password=QString());
    void disconnect();

private slots:
    void dataReady();
    void renewStatus();
    void statusReady();

    void onSocketError(QAbstractSocket::SocketError error);
    void onConnected();
    void onDisconnected();
    void sendNextRequest();

private:
    void enqueueRequest(MpdRequest *request);

private:
    QString m_host;
    int m_port;
    QString m_password;
    QTcpSocket *p_socket;

    MpdRequest *p_waitingRequest;
    QList<MpdRequest*> m_requestQueue;

    bool m_connected; //!< "OK MPD <version>" received?
    QSharedPointer<MpdStatus> p_status;

    QTimer *p_timer;
    
};


class MpdRequest : public QObject
{
    Q_OBJECT
public:
    MpdRequest(QString requestMessage) : m_requestMessage(requestMessage), m_ready(false) {}

    bool isResultReady() { return m_ready; }
    bool succesfull() { return m_ready?m_ack.isEmpty():false; }
    QString getAck() { return m_ack; }

    friend class MusicPlayerConnection;

signals:
    void resultReady();

protected:
    virtual void feedData(QByteArray) {} //default implementation discards the data
    void setOk() { m_ready = true; onCompleted(); emit resultReady(); }
    void setAck(QString msg) { m_ready = true; m_ack = msg; emit resultReady(); }
    virtual void onCompleted() {}

    QString m_requestMessage;

private:
    bool m_ready;
    QString m_ack;
};


class MpdEntityListRequest : public MpdRequest
{
    Q_OBJECT
public:
    MpdEntityListRequest(QString requestMessage) : MpdRequest(requestMessage) {}
    MpdEntityList getEntityList() { return m_list; }
protected:
    virtual void feedData(QByteArray data) { m_list.feedData(data); }
    virtual void onCompleted() { m_list.endData(); }
private:
    MpdEntityList m_list;
};


class MpdSongListRequest : public MpdRequest
{
    Q_OBJECT
public:
    MpdSongListRequest(QString requestMessage) : MpdRequest(requestMessage) {}
    MpdSongList getSongList() { return m_list; }
protected:
    virtual void feedData(QByteArray data) { m_list.feedData(data); }
    virtual void onCompleted() { m_list.endData(); }
private:
    MpdSongList m_list;
};

class MpdStatusRequest : public MpdRequest
{
    Q_OBJECT
public:
    MpdStatusRequest(QString requestMessage) : MpdRequest(requestMessage) {}
    QSharedPointer<MpdStatus> getStatus() { return p_status; }
protected:
    virtual void feedData(QByteArray data) { m_buffer.append(data); }
    virtual void onCompleted() { p_status = QSharedPointer<MpdStatus>(new MpdStatus(QString::fromUtf8(m_buffer.constData()))); }
private:
    QByteArray m_buffer;
    QSharedPointer<MpdStatus> p_status;
};

#endif // MUSICPLAYERCONNECTION_H
