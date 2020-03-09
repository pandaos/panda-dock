#include "dockitemmanager.h"
#include "item/launcheritem.h"
#include "item/showdesktopitem.h"
#include "item/pluginitem.h"
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
      m_windowManager(AppWindowManager::instance()),
      m_pluginsManager(new PluginsManager(this))
{
    m_itemList.append(new ShowDesktopItem);
    m_itemList.append(new LauncherItem);

    for (DockEntry *entry : m_windowManager->dockList()) {
        m_itemList.append(new AppItem(entry));
    }

    connect(m_windowManager, &AppWindowManager::entryAdded, this, &DockItemManager::appItemAdded);
    connect(m_windowManager, &AppWindowManager::entryRemoved, this, &DockItemManager::appItemRemoved);
    connect(m_windowManager, &AppWindowManager::activeChanged, this, &DockItemManager::appItemActiveChanged);

    connect(m_pluginsManager, &PluginsManager::pluginItemInserted, this, &DockItemManager::pluginItemInserted, Qt::QueuedConnection);
    connect(m_pluginsManager, &PluginsManager::pluginItemRemoved, this, &DockItemManager::pluginItemRemoved, Qt::QueuedConnection);
    connect(m_pluginsManager, &PluginsManager::pluginItemUpdated, this, &DockItemManager::itemUpdated, Qt::QueuedConnection);
}

const QList<QPointer<DockItem> > DockItemManager::itemList() const
{
    return m_itemList;
}

void DockItemManager::appItemAdded(DockEntry *entry)
{
    // 第一个是启动器
    int insertIndex = 1;
    // -1 表示插入最后
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
    for (DockItem *item : m_itemList) {
        if (item->itemType() != DockItem::App)
            continue;

        AppItem *app = static_cast<AppItem *>(item);
        if (!app)
            continue;

        app->update();
    }
}

void DockItemManager::pluginItemInserted(PluginItem *item)
{
    DockItem::ItemType pluginType = item->itemType();

    // find first plugins item position
    int firstPluginPosition = -1;
    for (int i(0); i != m_itemList.size(); ++i) {
        DockItem::ItemType type = m_itemList[i]->itemType();
        if (type != pluginType)
            continue;

        firstPluginPosition = i;
        break;
    }

    if (firstPluginPosition == -1)
        firstPluginPosition = m_itemList.size();

    // find insert position
    int insertIndex = m_itemList.size();;
    m_itemList.insert(insertIndex, item);

    emit itemInserted(insertIndex - firstPluginPosition, item);
}

void DockItemManager::pluginItemRemoved(PluginItem *item)
{
    emit itemRemoved(item);
    m_itemList.removeOne(item);
}
