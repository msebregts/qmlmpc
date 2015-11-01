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

#include "types.h"

#include <QStringList>

QHash<QString,QString> MPD::parseData(QString data)
{
    QHash<QString,QString> hash;
    QString tmp;
    int i;
    foreach (tmp, data.split("\n")) {
        i = tmp.indexOf(": ");
        if (i!=-1)
            hash.insertMulti(tmp.left(i).toLower(), tmp.mid(i+2));
    }
    return hash;
}


MpdStatus::MpdStatus(QString data)
{
    QHash<QString,QString> hash = MPD::parseData(data);
    m_repeat = hash.value("repeat") == "1";
    m_random = hash.value("random") == "1";
    m_playlistVersion = hash.value("playlist", "-1").toUInt();
    m_playlistLength = hash.value("playlistlength", "0").toUInt();
    QString state = hash.value("state");
    if (state == "stop")
        m_state = Stop;
    else if (state == "play")
        m_state = Play;
    else if (state == "pause")
        m_state = Pause;
    else {
        qDebug("Could not parse state '%s'", qPrintable(state));
        m_state = Unknown;
    }
    m_currentSongPos = hash.value("song", "-1").toInt();
    m_currentSongId = hash.value("songid", "-1").toInt();
    QString time = hash.value("time");
    if (time.contains(":")) {
        m_elapsedTime = time.section(':',0,0).toUInt();
        m_totalTime = time.section(':',1,1).toUInt();
    } else {
        m_elapsedTime = 0;
        m_totalTime = 0;
    }
    // ignore all other values, they aren't used anyway
}

bool MpdStatus::operator ==(MpdStatus &other)
{
    return (other.m_elapsedTime == m_elapsedTime && // put this one up front, because it is most likely to be the property that has changed
            other.m_repeat == m_repeat &&
            other.m_random == m_random &&
            other.m_playlistVersion == m_playlistVersion &&
            other.m_playlistLength == m_playlistLength &&
            other.m_state == m_state &&
            other.m_currentSongPos == m_currentSongPos &&
            other.m_currentSongId == m_currentSongId &&
            other.m_totalTime == m_totalTime);
}

MpdSong::MpdSong(QString data)
{
    if (!data.startsWith("file: ")) {
        qDebug("Can not create a song with data not starting with 'file: '");
        return;
    }
    QHash<QString,QString> hash = MPD::parseData(data);
    m_path = hash.value("file");
    m_title = hash.value("title");
    m_artist = hash.value("artist");
    m_album = hash.value("album");
    m_duration = hash.value("time", "-1").toInt();
    m_pos = hash.value("pos", "-1").toInt();
    m_id = hash.value("id", "-1").toInt();
}

QString MpdSong::getDescription()
{
    if (m_title.isEmpty() || m_artist.isEmpty())
        return m_path.section('/', -1);
    return m_title+ " - " + m_artist;
}

MpdDirectory::MpdDirectory(QString data)
{
    if (!data.startsWith("directory: ")) {
        qDebug("Can not create a directory with data not starting with 'directory: '");
        return;
    }
    if (data.indexOf('\n')!=-1) {
        qDebug("directory data contains more than one line, ignoring all but the first line");
        data = data.section('\n', 0, 0);
    }
    m_path = data.mid(11);
}

MpdPlaylist::MpdPlaylist(QString data)
{
    if (!data.startsWith("playlist: ")) {
        qDebug("Can not create a playlist width data not starting with 'playlist: '");
        return;
    }
    m_name = data.section('\n', 0, 0).mid(10); // ignore everything but the playlist name
}

QList<QSharedPointer<MpdEntity> > MpdEntityListParser::feedData(QByteArray data)
{
    m_buffer.append(data);
    QList<QSharedPointer<MpdEntity> > entities;
    // find first occurence of '\ndirectory: ', '\nfile: ' or
    // '\nplaylist: ', indicating the start of the next entity
    int i = -1;
    int lastIndex = 0;
    const char* buffer = m_buffer.constData();
    while ( (i=m_buffer.indexOf('\n', i+1)) != -1) {
        if (    qstrncmp(buffer+i+1, "directory: ", 11)==0 ||
                qstrncmp(buffer+i+1, "file: ", 6)==0  ||
                qstrncmp(buffer+i+1, "playlist: ", 10)==0 ) {
            QString entityData = QString::fromUtf8(buffer+lastIndex, i-lastIndex);
            lastIndex = i+1; // skip the \n (i++; lastIndex=i)
            MpdEntity *entity = 0;
            if (entityData.startsWith("file: "))
                entity = new MpdSong(entityData);
            else if (entityData.startsWith("directory: "))
                entity = new MpdDirectory(entityData);
            else if (entityData.startsWith("playlist: "))
                entity = new MpdPlaylist(entityData);
            else
                qDebug("Could not create entity from data '%s'", qPrintable(entityData));
            if (entity)
                entities.append(QSharedPointer<MpdEntity>(entity));
        }
    }
    m_buffer = m_buffer.mid(lastIndex);
    return entities;
}

QSharedPointer<MpdEntity> MpdEntityListParser::endData()
{
    MpdEntity *entity = 0;
    if (m_buffer.length()==0)
        return QSharedPointer<MpdEntity>();
    if (m_buffer.startsWith("file: "))
        entity = new MpdSong(QString::fromUtf8(m_buffer.constData()));
    else if (m_buffer.startsWith("directory: "))
        entity = new MpdDirectory(QString::fromUtf8(m_buffer.constData()));
    else if (m_buffer.startsWith("playlist: "))
        entity = new MpdPlaylist(QString::fromUtf8(m_buffer.constData()));
    if (!entity)
        qDebug("Could not create entity from data '%s'", m_buffer.constData());
    m_buffer.clear();
    return QSharedPointer<MpdEntity>(entity);
}

void MpdEntityList::endData()
{
    QSharedPointer<MpdEntity> entity = m_parser.endData();
    if (!entity.isNull())
        append(entity);
}

void MpdSongList::feedData(QByteArray data)
{
    QList<QSharedPointer<MpdEntity> > l = m_parser.feedData(data);
    for (int i=0; i<l.length(); i++)
        if (l.at(i)->getType()==MpdEntity::Song)
            append(qSharedPointerCast<MpdSong>(l.at(i)));
}

void MpdSongList::endData()
{
    QSharedPointer<MpdEntity> entity = m_parser.endData();
    if (entity.isNull())
        return;
    if (entity->getType()==MpdEntity::Song)
        append(qSharedPointerCast<MpdSong>(entity));
}

QSharedPointer<MpdSong> MpdSongList::getSongById(int songId)
{
    QSharedPointer<MpdSong> ptr;
    foreach (ptr, *this)
        if (ptr->getPlaylistId()==songId)
            return ptr;
    return QSharedPointer<MpdSong>();
}

int MpdSongList::getSongIndex(int songId)
{
    for (int i=0; i<length(); i++)
        if (at(i)->getPlaylistId()==songId)
            return i;
    return -1;
}

MpdSongList MpdSongList::fromEntityList(MpdEntityList l)
{
    MpdSongList songList;
    for (int i=0; i<l.length(); i++)
        if (l.at(i)->getType()==MpdEntity::Song)
            songList.append(qSharedPointerCast<MpdSong>(l.at(i)));
    return songList;
}

