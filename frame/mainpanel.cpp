#include "mainpanel.h"
#include <QPainter>
#include <QLabel>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_appAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_trayAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_pluginAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaWidget(new QWidget),
      m_appAreaWidget(new QWidget),
      m_trayAreaWidget(new QWidget),
      m_pluginAreaWidget(new QWidget),
      m_dockItemmanager(DockItemManager::instance()),
      m_settings(DockSettings::instance())
{
    setAttribute(Qt::WA_TranslucentBackground);

    init();
    updateLayout();

    connect(m_dockItemmanager, &DockItemManager::itemInserted, this, &MainPanel::insertItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemRemoved, this, &MainPanel::removeItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemUpdated, this, &MainPanel::itemUpdated, Qt::QueuedConnection);
    connect(m_settings, &DockSettings::positionChanged, this, &MainPanel::onPositionChanged);
}

void MainPanel::addFixedAreaItem(int index, QWidget *wdg)
{
    m_fixedAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
}

void MainPanel::addAppAreaItem(int index, QWidget *wdg)
{
    m_appAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
}

void MainPanel::addTrayAreaItem(int index, QWidget *wdg)
{
    m_trayAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
}

void MainPanel::addPluginAreaItem(int index, QWidget *wdg)
{
    m_pluginAreaLayout->insertWidget(index, wdg, 0, Qt::AlignCenter);
    resizeDockIcon();
    m_pluginAreaWidget->adjustSize();
}

void MainPanel::removeFixedAreaItem(QWidget *wdg)
{
    m_fixedAreaLayout->removeWidget(wdg);
}

void MainPanel::removeAppAreaItem(QWidget *wdg)
{
    m_appAreaLayout->removeWidget(wdg);
}

void MainPanel::removeTrayAreaItem(QWidget *wdg)
{
    m_trayAreaLayout->removeWidget(wdg);
}

void MainPanel::removePluginAreaItem(QWidget *wdg)
{
    m_pluginAreaLayout->removeWidget(wdg);
}

void MainPanel::insertItem(const int index, DockItem *item)
{
    item->installEventFilter(this);

    switch (item->itemType()) {
    case DockItem::Launcher:
        addFixedAreaItem(0, item);
        break;
    case DockItem::FixedPlugin:
        addFixedAreaItem(index, item);
        break;
    case DockItem::App:
    case DockItem::Placeholder:
        addAppAreaItem(index, item);
        break;
    case DockItem::TrayPlugin:
        addTrayAreaItem(index, item);
        break;
    case DockItem::Plugins:
        addPluginAreaItem(index, item);
        break;
    default:
        break;
    }
}

void MainPanel::removeItem(DockItem *item)
{
    switch (item->itemType()) {
    case DockItem::Launcher:
    case DockItem::FixedPlugin:
        removeFixedAreaItem(item);
        break;
    case DockItem::App:
    case DockItem::Placeholder:
        removeAppAreaItem(item);
        break;
    case DockItem::TrayPlugin:
        removeTrayAreaItem(item);
        break;
    case DockItem::Plugins:
        removePluginAreaItem(item);
        break;
    default:
        break;
    }
}

void MainPanel::itemUpdated(DockItem *item)
{
    item->parentWidget()->adjustSize();
    resizeDockIcon();
}

void MainPanel::init()
{
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_fixedAreaWidget);
    m_mainLayout->addSpacing(5);
    m_mainLayout->addWidget(m_appAreaWidget);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_trayAreaWidget);
    m_mainLayout->addWidget(m_pluginAreaWidget);

    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(10, 0, 10, 0);
    m_mainLayout->setSpacing(0);

    m_fixedAreaWidget->setLayout(m_fixedAreaLayout);
    m_fixedAreaLayout->setMargin(0);
    m_fixedAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_fixedAreaLayout->setSpacing(0);

    m_appAreaWidget->setLayout(m_appAreaLayout);
    m_appAreaLayout->setMargin(0);
    m_appAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_appAreaLayout->setSpacing(0);

    m_trayAreaWidget->setLayout(m_trayAreaLayout);
    m_trayAreaLayout->setMargin(0);
    m_trayAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_trayAreaLayout->setSpacing(0);

    m_pluginAreaWidget->setLayout(m_pluginAreaLayout);
    m_pluginAreaLayout->setMargin(0);
    m_pluginAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_pluginAreaLayout->setSpacing(0);

    setLayout(m_mainLayout);
}

void MainPanel::updateLayout()
{
    switch (m_settings->position()) {
    case DockSettings::Left:
    case DockSettings::Right:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_pluginAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_trayAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_mainLayout->setAlignment(m_appAreaWidget, Qt::AlignTop);
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
        m_fixedAreaLayout->setDirection(QBoxLayout::TopToBottom);
        m_appAreaLayout->setDirection(QBoxLayout::TopToBottom);
        m_trayAreaLayout->setContentsMargins(0, 10, 0, 10);
        break;
    case DockSettings::Bottom:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_pluginAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_trayAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_mainLayout->setAlignment(m_appAreaWidget, Qt::AlignLeft);
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
        m_fixedAreaLayout->setDirection(QBoxLayout::LeftToRight);
        m_appAreaLayout->setDirection(QBoxLayout::LeftToRight);
        m_trayAreaLayout->setContentsMargins(0, 10, 0, 10);
        break;
    }
}

void MainPanel::resizeDockIcon()
{
    DockSettings::Position pos = m_settings->position();
    int totalLength = (pos == DockSettings::Bottom) ? width() : height();

    if (totalLength < 0)
        return;

    int iconCount = m_fixedAreaLayout->count() + m_appAreaLayout->count();
    int pluginCount = 0;
    int iconSize = 0;

    int yu = (totalLength % iconCount);
    iconSize = (totalLength - yu) / iconCount;

    if (iconSize < 20 || iconSize > 40) {
        if (iconSize < 20)
            totalLength -= 20 * pluginCount;
        else
            totalLength -= 40 * pluginCount;

        iconCount -= pluginCount;

        int yu = (totalLength % iconCount);
        iconSize = (totalLength - yu) / iconCount;
    }

    if (pos == DockSettings::Bottom) {
        if (iconSize >= height()) {
            calcuDockIconSize(height(), height());
        } else {
            calcuDockIconSize(iconSize, height());
        }
    } else {
        if (iconSize >= width()) {
            calcuDockIconSize(width(), width());
        } else {
            calcuDockIconSize(width(), iconSize);
        }
    }
}

void MainPanel::calcuDockIconSize(int w, int h)
{
//    for (int i = 0; i < m_fixedAreaLayout->count(); ++ i) {
//        m_fixedAreaLayout->itemAt(i)->widget()->setFixedSize(w, h);
//    }

    DockSettings::Position pos = m_settings->position();
    if (pos == DockSettings::Bottom) {
        for (int i = 0; i < m_appAreaLayout->count(); ++ i) {
            m_appAreaLayout->itemAt(i)->widget()->setMaximumWidth(h);
            m_appAreaLayout->itemAt(i)->widget()->setMaximumHeight(QWIDGETSIZE_MAX);
        }

        for (int i = 0; i < m_fixedAreaLayout->count(); ++ i) {
            m_fixedAreaLayout->itemAt(i)->widget()->setMaximumWidth(h);
            m_fixedAreaLayout->itemAt(i)->widget()->setMaximumHeight(QWIDGETSIZE_MAX);
        }
    } else {
        for (int i = 0; i < m_appAreaLayout->count(); ++ i) {
            m_appAreaLayout->itemAt(i)->widget()->setMaximumHeight(w);
            m_appAreaLayout->itemAt(i)->widget()->setMaximumWidth(QWIDGETSIZE_MAX);
        }

        for (int i = 0; i < m_fixedAreaLayout->count(); ++ i) {
            m_fixedAreaLayout->itemAt(i)->widget()->setMaximumHeight(w);
            m_fixedAreaLayout->itemAt(i)->widget()->setMaximumWidth(QWIDGETSIZE_MAX);
        }
    }
}

void MainPanel::onPositionChanged()
{
    updateLayout();
}

void MainPanel::resizeEvent(QResizeEvent *event)
{
    resizeDockIcon();
    QWidget::resizeEvent(event);
}
