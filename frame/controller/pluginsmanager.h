#ifndef PLUGINSMANAGER_H
#define PLUGINSMANAGER_H

#include <QObject>
#include "item/pluginitem.h"

class DockItemManager;
class PluginProxyInterface;
class PluginsManager : public QObject, PluginProxyInterface
{
    Q_OBJECT

    friend class DockItemManager;

public:
    explicit PluginsManager(DockItemManager *parent = nullptr);

    void startLoader();

    QMap<PluginsItemInterface *, QMap<QString, QObject *>> &pluginsMap();

    // implements PluginProxyInterface
    void itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey);
    void itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey);
    void itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey);
    void requestContextMenu(PluginsItemInterface * const itemInter, const QString &itemKey);

signals:
    void pluginItemInserted(PluginItem *pluginItem) const;
    void pluginItemRemoved(PluginItem *pluginItem) const;
    void pluginItemUpdated(PluginItem *pluginItem) const;

private:
    void loadLocalPlugins();
    void loadSystemPlugins();
    void loadPlugin(const QString &fileName);
    QObject *pluginItemAt(PluginsItemInterface * const itemInter, const QString &itemKey) const;

private:
    QMap<PluginsItemInterface *, QMap<QString, QObject *>> m_pluginsMap;
};

#endif // PLUGINSMANAGER_H
