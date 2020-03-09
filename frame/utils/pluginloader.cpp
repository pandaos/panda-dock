#include "pluginloader.h"

#include <QDir>
#include <QDebug>
#include <QLibrary>

PluginLoader::PluginLoader(const QString &pluginDirPath, QObject *parent)
    : QThread(parent)
    , m_pluginDirPath(pluginDirPath)
{
}

void PluginLoader::run()
{
    QDir pluginsDir(m_pluginDirPath);
    const QStringList plugins = pluginsDir.entryList(QDir::Files);

    for (const QString file : plugins) {
        if (!QLibrary::isLibrary(file))
            continue;

        if (file.startsWith("libpandaos-dock-"))
            continue;

        emit pluginFounded(pluginsDir.absoluteFilePath(file));
    }

    emit finished();
}
