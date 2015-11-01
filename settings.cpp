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

#include "settings.h"

#include <QDebug>

#define setDefaultSetting(key, value) if (!p_settings->contains(key)) { p_settings->setValue(key, value); }

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    p_settings = new QSettings("qmlmpc", "qmlmpc", this);
    // mpd connection
    setDefaultSetting("mpd/host", "localhost");
    setDefaultSetting("mpd/port", 6600);
    setDefaultSetting("mpd/password", "");
    p_settings->sync();
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    p_settings->setValue(key, value);
    p_settings->sync();
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return p_settings->value(key, defaultValue);
}
