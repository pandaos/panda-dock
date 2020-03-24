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

#ifndef DOCKITEMMANAGER_H
#define DOCKITEMMANAGER_H

#include "item/dockitem.h"
#include "item/appitem.h"
#include "appwindowmanager.h"

#include <QObject>
#include <QPointer>

class DockItemManager : public QObject
{
    Q_OBJECT

public:
    static DockItemManager *instance(QObject *parent = nullptr);
    explicit DockItemManager(QObject *parent = nullptr);

    const QList<QPointer<DockItem>> itemList() const;

private:
    void appItemAdded(DockEntry *entry);
    void appItemRemoved(DockEntry *entry);
    void appItemActiveChanged(DockEntry *entry);

signals:
    void itemInserted(const int index, DockItem *item) const;
    void itemRemoved(DockItem *item) const;
    void itemUpdated(DockItem *item) const;
    void trayVisableCountChanged(const int &count) const;
    void requestWindowAutoHide(const bool autoHide) const;
    void requestRefershWindowVisible() const;

private:
    static DockItemManager *INSTANCE;
    AppWindowManager *m_windowManager;
    QList<QPointer<DockItem>> m_itemList;
};

#endif // DOCKITEMMANAGER_H
