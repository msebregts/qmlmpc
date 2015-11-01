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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "settings.h"
#include "mpc/mpdconnector.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Settings *settings = new Settings(&app);

    MpdConnector *mpdConnector = new MpdConnector(settings->value("mpd/host").toString(), settings->value("mpd/port").toInt(), settings->value("mpd/password").toString());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("mpdConnector", mpdConnector);
    engine.rootContext()->setContextProperty("settings", settings);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
