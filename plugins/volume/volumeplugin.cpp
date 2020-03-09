#include "volumeplugin.h"
#include <QLabel>

VolumePlugin::VolumePlugin(QObject *parent)
    : QObject(parent),
      m_widget(new VolumeWidget)
{

}

const QString VolumePlugin::pluginName() const
{
    return "volume";
}

const QString VolumePlugin::pluginDisplayName() const
{
    return tr("Volume");
}

void VolumePlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    m_proxyInter->itemAdded(this, QString());
}

QWidget *VolumePlugin::itemWidget(const QString &itemKey)
{
    return m_widget;
}

QWidget *VolumePlugin::itemTipsWidget(const QString &itemKey)
{
    return nullptr;
}
