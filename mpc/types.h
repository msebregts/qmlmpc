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

#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QHash>
#include <QList>
#include <QSharedPointer>

namespace MPD {
QHash<QString, QString> parseData(QString data);
}

class MpdStatus
{
public:
    enum MpdState {
        Play,
        Pause,
        Stop,
        Unknown
    };

    MpdStatus(QString data);

    bool repeat() { return m_repeat; }
    bool random() { return m_random; }
    int playlistVersion() { return m_playlistVersion; }
    int playlistLength() { return m_playlistLength; }
    MpdState state() { return m_state; }
    int currentSongPos() { return m_currentSongPos; }
    int currentSongId() { return m_currentSongId; }
    unsigned int elapsedTime() { return m_elapsedTime; }
    unsigned int totalTime() { return m_totalTime; }

    bool operator==(MpdStatus &other);
    bool operator!=(MpdStatus &other) { return !operator ==(other); }

private:
    bool m_repeat;
    bool m_random;
    int m_playlistVersion;
    int m_playlistLength;
    MpdState m_state;
    int m_currentSongPos;
    int m_currentSongId;
    unsigned int m_elapsedTime;
    unsigned int m_totalTime;
};

class MpdEntity
{
public:
    enum Type {
        Song,
        Directory,
        Playlist
    };

    virtual ~MpdEntity() {}
    virtual QString getDescription() = 0;
    virtual Type getType() = 0;
    virtual QString getPath() = 0;
};


class MpdSong : public MpdEntity
{
public:
    MpdSong(QString data);
    virtual Type getType() { return Song; }
    virtual QString getDescription();

    QString getPath() { return m_path; }
    QString getTitle() { return m_title; }
    QString getArtist() { return m_artist; }
    QString getAlbum() { return m_album; }
    int getDuration() { return m_duration; }
    int getPlaylistPosition() { return m_pos; }
    int getPlaylistId() { return m_id; }

private:
    QString m_path;
    QString m_title;
    QString m_artist;
    QString m_album;
    int m_duration;
    int m_pos;
    int m_id;
};


class MpdDirectory : public MpdEntity
{
public:
    MpdDirectory(QString data);
    virtual Type getType() { return Directory; }
    virtual QString getDescription() { return m_path.section('/', -1); }
    QString getPath() { return m_path; }

private:
    QString m_path;
};

class MpdPlaylist : public MpdEntity
{
public:
    MpdPlaylist(QString data);
    Type getType() { return Playlist; }
    virtual QString getDescription() { return m_name; }
    QString getName() { return m_name; }
    QString getPath() { return m_name; }

private:
    QString m_name;
};


class MpdEntityListParser
{
public:
    MpdEntityListParser(QByteArray data=QByteArray()) { feedData(data); }
    QList<QSharedPointer<MpdEntity> > feedData(QByteArray data);
    QSharedPointer<MpdEntity> endData();
private:
    QByteArray m_buffer;
};


class MpdEntityList : public QList<QSharedPointer<MpdEntity> >
{
public:
    MpdEntityList(QByteArray data=QByteArray()) { feedData(data); }
    void feedData(QByteArray data) { append(m_parser.feedData(data)); }
    void endData();
private:
    MpdEntityListParser m_parser;
};

class MpdSongList : public QList<QSharedPointer<MpdSong> >
{
public:
    MpdSongList(QByteArray data=QByteArray()) { feedData(data); }
    void feedData(QByteArray data);
    void endData();
    QSharedPointer<MpdSong> getSongById(int songId);
    int getSongIndex(int songId);
    static MpdSongList fromEntityList(MpdEntityList l);
private:
    MpdEntityListParser m_parser;
};


#endif // TYPES_H
