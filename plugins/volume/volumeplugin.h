#ifndef VOLUMEPLUGIN_H
#define VOLUMEPLUGIN_H

#include <QObject>
#include <QLabel>
#include <QTimer>

#include "volumewidget.h"
#include "pluginsiteminterface.h"

class VolumePlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.pandaos.dock.PluginsItemInterface" FILE "volume.json")

public:
    explicit VolumePlugin(QObject *parent = nullptr);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;

    void init(PluginProxyInterface *proxyInter) override;

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;

private:
    VolumeWidget *m_widget;
};

#endif // VOLUMEPLUGIN_H
