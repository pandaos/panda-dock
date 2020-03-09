#include "pluginsmanager.h"
#include "pluginproxyinterface.h"
#include "dockitemmanager.h"
#include <QPluginLoader>
#include <QDebug>
#include <QFile>
#include <QDir>

PluginsManager::PluginsManager(DockItemManager *parent)
    : QObject(parent)
{
    QTimer::singleShot(1000, this, &PluginsManager::startLoader);
}

void PluginsManager::startLoader()
{
    loadLocalPlugins();
//    loadSystemPlugins();
}

QMap<PluginsItemInterface *, QMap<QString, QObject *> > &PluginsManager::pluginsMap()
{
    return m_pluginsMap;
}

void PluginsManager::itemAdded(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    // check if same item added
    if (m_pluginsMap.contains(itemInter))
        if (m_pluginsMap[itemInter].contains(itemKey))
            return;

    PluginItem *item = new PluginItem(itemInter, itemKey);
    m_pluginsMap[itemInter][itemKey] = item;
    emit pluginItemInserted(item);
}

void PluginsManager::itemUpdate(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    PluginItem *item = static_cast<PluginItem *>(pluginItemAt(itemInter, itemKey));

    Q_ASSERT(item);

    item->update();

    emit pluginItemUpdated(item);
}

void PluginsManager::itemRemoved(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    PluginItem *item = static_cast<PluginItem *>(pluginItemAt(itemInter, itemKey));
    if (!item)
        return;

    item->detachPluginWidget();

    emit pluginItemRemoved(item);
    m_pluginsMap[itemInter].remove(itemKey);

    // do not delete the itemWidget object(specified in the plugin interface)
    item->centralWidget()->setParent(nullptr);

    // just delete our wrapper object(PluginsItem)
    item->deleteLater();
}

void PluginsManager::requestContextMenu(PluginsItemInterface * const itemInter, const QString &itemKey)
{
    PluginItem *item = static_cast<PluginItem *>(pluginItemAt(itemInter, itemKey));
    if (!item)
        return;
}

void PluginsManager::loadLocalPlugins()
{
    QString pluginsDirPath = QString("%1/.local/lib/ibyte-dock/plugins/").arg(QDir::homePath());
    QDir pluginsDir(pluginsDirPath);

    if (!pluginsDir.exists())
        return;

    qDebug() << "load local plugins: " << pluginsDirPath;

    for (QFileInfo &info : pluginsDir.entryInfoList()) {
        if (!QLibrary::isLibrary(info.absoluteFilePath()))
            continue;

        loadPlugin(info.absoluteFilePath());
    }
}

void PluginsManager::loadSystemPlugins()
{
    QString pluginsDir(qApp->applicationDirPath() + "/../plugins");

//    if (!QDir(pluginsDir).exists()) {
//        pluginsDir = "/usr/lib/ibyte-dock/plugins";
//    }

    qDebug() << "load plugins: " << pluginsDir;
}

void PluginsManager::loadPlugin(const QString &fileName)
{
    QPluginLoader *loader = new QPluginLoader(fileName);
    const auto meta = loader->metaData().value("MetaData").toObject();
    if (!meta.contains("api") || meta["api"].toString() != "1.0") {
        qWarning() << "plugin api version not matched!" << fileName;
        return;
    }

    PluginsItemInterface *interface = qobject_cast<PluginsItemInterface *>(loader->instance());

    if (!interface) {
        qWarning() << "load plugin failed, error: " << loader->errorString();
        loader->unload();
        loader->deleteLater();
        return;
    }

    qDebug() << "load plugin: " << fileName;

    m_pluginsMap.insert(interface, QMap<QString, QObject *>());
    QTimer::singleShot(1, this, [=] {
        interface->init(this);
    });
}

QObject *PluginsManager::pluginItemAt(PluginsItemInterface * const itemInter, const QString &itemKey) const
{
    if (!m_pluginsMap.contains(itemInter))
        return nullptr;

    return m_pluginsMap[itemInter][itemKey];
}
