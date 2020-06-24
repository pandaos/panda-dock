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

#include "dockitemmanager.h"
#include "item/launcheritem.h"
#include <QDebug>

DockItemManager *DockItemManager::INSTANCE = nullptr;

DockItemManager *DockItemManager::instance(QObject *parent)
{
    if (!INSTANCE)
        INSTANCE = new DockItemManager(parent);

    return INSTANCE;
}

DockItemManager::DockItemManager(QObject *parent)
    : QObject(parent),
      m_windowManager(AppWindowManager::instance())
{
    for (DockEntry *entry : m_windowManager->dockList()) {
        m_itemList.append(new AppItem(entry));
    }

    connect(m_windowManager, &AppWindowManager::entryAdded, this, &DockItemManager::appItemAdded);
    connect(m_windowManager, &AppWindowManager::entryRemoved, this, &DockItemManager::appItemRemoved);
    connect(m_windowManager, &AppWindowManager::activeChanged, this, &DockItemManager::appItemActiveChanged);
}

const QList<QPointer<DockItem>> DockItemManager::itemList() const
{
    return m_itemList;
}

void DockItemManager::appItemAdded(DockEntry *entry)
{
    int insertIndex = 1;
    int index = -1;

    // -1 for append to app list end
    if (index != -1) {
        insertIndex += index;
    } else {
        for (auto item : m_itemList)
            if (item->itemType() == DockItem::App)
                ++insertIndex;
    }

    AppItem *item = new AppItem(entry);
    m_itemList.insert(insertIndex, item);

    if (index != -1) {
        emit itemInserted(insertIndex - 1, item);
        return;
    }

    emit itemInserted(insertIndex, item);
}

void DockItemManager::appItemRemoved(DockEntry *entry)
{
    for (int i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList.at(i)->itemType() != DockItem::App)
            continue;

        AppItem *app = static_cast<AppItem *>(m_itemList.at(i).data());
        if (!app)
            continue;

        if (app->entry() == entry) {
            emit itemRemoved(app);
            m_itemList.removeOne(app);
            app->deleteLater();
        }
    }
}

void DockItemManager::appItemActiveChanged(DockEntry *entry)
{
    Q_UNUSED(entry);

    for (DockItem *item : m_itemList) {
        if (item->itemType() != DockItem::App)
            continue;

        AppItem *app = static_cast<AppItem *>(item);
        if (!app)
            continue;

        app->update();
    }
}
