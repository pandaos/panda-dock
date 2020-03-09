#ifndef PLUGINITEM_H
#define PLUGINITEM_H

#include "dockitem.h"
#include "pluginsiteminterface.h"

class PluginItem : public DockItem
{
    Q_OBJECT

public:
    explicit PluginItem(PluginsItemInterface *const pluginInter, const QString &itemKey, QWidget *parent = nullptr);

    inline ItemType itemType() const override { return DockItem::Plugins; }

    void detachPluginWidget();

    QSize sizeHint() const override;
    QString pluginName() const;

    QWidget *centralWidget() const;

public slots:
    void refershIcon();

private:
    PluginsItemInterface *const m_pluginInter;
    const QString m_itemKey;
    QWidget *m_centralWidget;
};

#endif // PLUGINITEM_H
