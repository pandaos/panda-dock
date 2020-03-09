#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include <QObject>
#include <QLabel>
#include <QTimer>

#include "pluginsiteminterface.h"

class DatetimePlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.pandaos.dock.PluginsItemInterface" FILE "datetime.json")

public:
    explicit DatetimePlugin(QObject *parent = nullptr);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;

    void init(PluginProxyInterface *proxyInter) override;

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;

private:
    void updateCurrentTimeString();

private:
    QLabel *m_datetimeLabel;
    QTimer *m_refreshTimer;
    QString m_currentTimeString;
};

#endif // DATETIMEPLUGIN_H
