#ifndef DOCKITEMMANAGER_H
#define DOCKITEMMANAGER_H

#include "pluginsmanager.h"
#include "pluginsiteminterface.h"
#include "item/dockitem.h"
#include "item/appitem.h"
#include "item/pluginitem.h"
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

    void pluginItemInserted(PluginItem *item);
    void pluginItemRemoved(PluginItem *item);

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
    PluginsManager *m_pluginsManager;
    QList<QPointer<DockItem>> m_itemList;
};

#endif // DOCKITEMMANAGER_H
