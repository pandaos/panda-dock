#include "pluginitem.h"
#include <QBoxLayout>
#include <QDebug>

PluginItem::PluginItem(PluginsItemInterface *const pluginInter, const QString &itemKey, QWidget *parent)
    : DockItem(parent),
      m_pluginInter(pluginInter),
      m_itemKey(itemKey),
      m_centralWidget(m_pluginInter->itemWidget(itemKey))
{
    qDebug() << "load plugins item: " << pluginInter->pluginName() << itemKey << m_centralWidget;

    m_centralWidget->setParent(this);
    m_centralWidget->setVisible(true);
    m_centralWidget->installEventFilter(this);

    QBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_centralWidget);
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    setLayout(hLayout);
    setAccessibleName(pluginInter->pluginName() + "-" + m_itemKey);
    setAttribute(Qt::WA_TranslucentBackground);
}

void PluginItem::detachPluginWidget()
{
    QWidget *widget = m_pluginInter->itemWidget(m_itemKey);

    if (widget)
        widget->setParent(nullptr);
}

QSize PluginItem::sizeHint() const
{
    return m_centralWidget->sizeHint();
}

QString PluginItem::pluginName() const
{
    return m_pluginInter->pluginName();
}

QWidget *PluginItem::centralWidget() const
{
    return m_centralWidget;
}

void PluginItem::refershIcon()
{
    m_pluginInter->refreshIcon(m_itemKey);
}
