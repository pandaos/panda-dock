/*
 * Copyright (C) 2020 PandaOS Team.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef APPWINDOWMANAGER_H
#define APPWINDOWMANAGER_H

#include <QObject>
#include <QSettings>
#include <QDataStream>
#include <QSet>
#include <KF5/KWindowSystem/KWindowSystem>
#include <KF5/KWindowSystem/KWindowInfo>
#include <KF5/KWindowSystem/NETWM>

struct DockEntry
{
    bool isActive = false;
    bool isDocked = false;
    QString desktopPath;
    QString className;
    QString iconName;
    QString exec;
    QString name;
    QString id;
    int current = 0;

    QList<quint64> WIdList;
};

class AppWindowManager : public QObject
{
    Q_OBJECT

public:
    static AppWindowManager *instance();
    explicit AppWindowManager(QObject *parent = nullptr);

    QList<DockEntry *> dockList() const { return m_dockList; }
    DockEntry *find(quint64 id);
    bool classNameContains(const QString &className);
    bool isAcceptWindow(quint64 id) const;

    void triggerWindow(quint64 id);
    void minimizeWindow(quint64 id);
    void raiseWindow(quint64 id);
    void closeWindow(quint64 id);

    void openApp(const QString &appName);
    void clicked(DockEntry *entry);
    void undock(DockEntry *entry);
    void save();

signals:
    void entryAdded(DockEntry *entry);
    void entryRemoved(DockEntry *entry);
    void activeChanged(DockEntry *entry);

private:
    void refreshWindowList();
    QString getExec(quint64 id);
    QString whichCmd(const QString &cmd);
    QString getDesktop(const QString &keyword);
    QString getIcon(const QString &exec);
    void onWindowAdded(quint64 id);
    void onWindowRemoved(quint64 id);
    void onActiveWindowChanged(quint64 id);
    void onWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);

private:
    QSettings *m_settings;
    int m_currentDesktop;
    QList<DockEntry *> m_dockList;
};

#endif // APPWINDOWMANAGER_H
