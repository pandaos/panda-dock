#include "mainpanel.h"
#include <QPainter>
#include <QLabel>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_appAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaWidget(new QWidget),
      m_appAreaWidget(new QWidget),
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

void MainPanel::removeFixedAreaItem(QWidget *wdg)
{
    m_fixedAreaLayout->removeWidget(wdg);
}

void MainPanel::removeAppAreaItem(QWidget *wdg)
{
    m_appAreaLayout->removeWidget(wdg);

    emit requestResized();
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
        addAppAreaItem(index, item);
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
        removeAppAreaItem(item);
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
    m_mainLayout->addWidget(m_fixedAreaWidget);
    m_mainLayout->addWidget(m_appAreaWidget);

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

    setLayout(m_mainLayout);
}

void MainPanel::updateLayout()
{
    switch (m_settings->position()) {
    case DockSettings::Left:
    case DockSettings::Right:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setAlignment(m_appAreaWidget, Qt::AlignTop);
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
        m_fixedAreaLayout->setDirection(QBoxLayout::TopToBottom);
        m_appAreaLayout->setDirection(QBoxLayout::TopToBottom);
        break;
    case DockSettings::Bottom:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setAlignment(m_appAreaWidget, Qt::AlignLeft);
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
        m_fixedAreaLayout->setDirection(QBoxLayout::LeftToRight);
        m_appAreaLayout->setDirection(QBoxLayout::LeftToRight);
        break;
    }
}

void MainPanel::resizeDockIcon()
{
    const int iconSize = m_settings->iconSize();

    for (int i = 0; i < m_appAreaLayout->count(); ++ i) {
        m_appAreaLayout->itemAt(i)->widget()->setFixedSize(iconSize, iconSize);
    }

    for (int i = 0; i < m_fixedAreaLayout->count(); ++ i) {
        m_fixedAreaLayout->itemAt(i)->widget()->setFixedSize(iconSize, iconSize);
    }

    emit requestResized();
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
