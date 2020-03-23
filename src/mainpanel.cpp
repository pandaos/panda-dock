#include "mainpanel.h"
#include "item/trashitem.h"
#include <QPainter>
#include <QLabel>
#include <QDebug>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
//      m_appAreaLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_fixedAreaWidget(new QWidget),
//      m_appAreaWidget(new QWidget),
      m_appArea(new AppScrollArea),
      m_trashItem(new TrashItem),
      m_dockItemmanager(DockItemManager::instance()),
      m_settings(DockSettings::instance())
{
    setAttribute(Qt::WA_TranslucentBackground);

    init();
    updateLayout();

    connect(m_dockItemmanager, &DockItemManager::itemInserted, this, &MainPanel::insertItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemRemoved, this, &MainPanel::removeItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemUpdated, this, &MainPanel::itemUpdated, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::requestScrollToItem, this, &MainPanel::scrollToItem);
    connect(m_settings, &DockSettings::positionChanged, this, &MainPanel::onPositionChanged);
}

void MainPanel::addFixedAreaItem(int index, QWidget *wdg)
{
    m_fixedAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
}

void MainPanel::addAppAreaItem(int index, QWidget *wdg)
{
//    m_appAreaLayout->insertWidget(index, wdg);
    resizeDockIcon();
}

void MainPanel::removeFixedAreaItem(QWidget *wdg)
{
    m_fixedAreaLayout->removeWidget(wdg);
}

void MainPanel::removeAppAreaItem(QWidget *wdg)
{
//    m_appAreaLayout->removeWidget(wdg);

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
        // addAppAreaItem(index, item);
        m_appArea->addItem(static_cast<AppItem *>(item));
        resizeDockIcon();
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
        // removeAppAreaItem(item);
        m_appArea->removeItem(static_cast<AppItem *>(item));
        resizeDockIcon();
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
//    m_mainLayout->addWidget(m_appAreaWidget);
    m_mainLayout->addWidget(m_appArea);
    m_mainLayout->addWidget(m_trashItem);
    m_mainLayout->addStretch();

    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_fixedAreaWidget->setLayout(m_fixedAreaLayout);
    m_fixedAreaLayout->setMargin(0);
    m_fixedAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_fixedAreaLayout->setSpacing(0);

//    m_appAreaWidget->setLayout(m_appAreaLayout);
//    m_appAreaLayout->setMargin(0);
//    m_appAreaLayout->setContentsMargins(0, 0, 0, 0);
//    m_appAreaLayout->setSpacing(0);

    setLayout(m_mainLayout);
}

void MainPanel::updateLayout()
{
    switch (m_settings->position()) {
    case DockSettings::Left:
    case DockSettings::Right:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
        m_fixedAreaLayout->setDirection(QBoxLayout::TopToBottom);
//        m_appAreaLayout->setDirection(QBoxLayout::TopToBottom);
        m_appArea->layout()->setDirection(QBoxLayout::TopToBottom);
        m_mainLayout->setContentsMargins(0, 10, 0, 10);
        break;
    case DockSettings::Bottom:
        m_fixedAreaWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // m_appArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
        m_fixedAreaLayout->setDirection(QBoxLayout::LeftToRight);
//        m_appAreaLayout->setDirection(QBoxLayout::LeftToRight);
        m_appArea->layout()->setDirection(QBoxLayout::LeftToRight);
        m_mainLayout->setContentsMargins(10, 0, 10, 0);
        break;
    }

    resizeDockIcon();

    emit requestResized();
}

void MainPanel::resizeDockIcon()
{
    int iconSize = m_settings->iconSize();
    const int padding = 10;

    m_appArea->setRange(iconSize);

    for (int i = 0; i < m_appArea->layout()->count(); ++i) {
        m_appArea->layout()->itemAt(i)->widget()->setFixedSize(iconSize, iconSize);
    }

    int canDisplayAppCount = 0;

    if (m_settings->position() == DockSettings::Bottom) {
        canDisplayAppCount = (rect().width() / iconSize) - 2;
        // rect().width() - iconSize * 2 - padding * 4
        m_appArea->setFixedWidth(canDisplayAppCount * iconSize);
        m_appArea->setFixedHeight(QWIDGETSIZE_MAX);
    } else {
        canDisplayAppCount = (rect().height() / iconSize) - 2;
        m_appArea->setFixedWidth(QWIDGETSIZE_MAX);
        m_appArea->setFixedHeight(canDisplayAppCount * iconSize);
    }

    for (int i = 0; i < m_fixedAreaLayout->count(); ++i) {
        m_fixedAreaLayout->itemAt(i)->widget()->setFixedSize(iconSize, iconSize);
    }

    m_trashItem->setFixedSize(iconSize, iconSize);

    emit requestResized();
}

void MainPanel::onPositionChanged()
{
    updateLayout();
}

void MainPanel::scrollToItem(DockItem *item)
{
    m_appArea->scrollToItem(static_cast<AppItem *>(item));
}

void MainPanel::resizeEvent(QResizeEvent *event)
{
    resizeDockIcon();
    QWidget::resizeEvent(event);
}
