/*
 * Copyright (C) 2020 PandaOS Team.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainpanel.h"
#include "item/trashitem.h"
#include "item/launcheritem.h"
#include <QPainter>
#include <QLabel>
#include <QDebug>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_appArea(new AppScrollArea),
      m_launcherItem(new LauncherItem),
      m_trashItem(new TrashItem),
      m_dockItemmanager(DockItemManager::instance()),
      m_settings(DockSettings::instance())
{
    m_mainLayout->addWidget(m_launcherItem);
    m_mainLayout->addWidget(m_appArea);
    m_mainLayout->addWidget(m_trashItem);

    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setAttribute(Qt::WA_TranslucentBackground);
    setLayout(m_mainLayout);
    updateLayout();

    connect(m_dockItemmanager, &DockItemManager::itemInserted, this, &MainPanel::insertItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemRemoved, this, &MainPanel::removeItem, Qt::QueuedConnection);
    connect(m_dockItemmanager, &DockItemManager::itemUpdated, this, &MainPanel::itemUpdated, Qt::QueuedConnection);
    connect(m_settings, &DockSettings::positionChanged, this, &MainPanel::onPositionChanged);
}

void MainPanel::insertItem(const int index, DockItem *item)
{
    Q_UNUSED(index);

    AppItem *appItem = static_cast<AppItem *>(item);
    if (appItem) {
        appItem->installEventFilter(this);
        m_appArea->addItem(appItem);
        resizeDockIcon();
    }
}

void MainPanel::removeItem(DockItem *item)
{
    AppItem *appItem = static_cast<AppItem *>(item);
    if (appItem) {
        m_appArea->removeItem(appItem);
        resizeDockIcon();
    }
}

void MainPanel::itemUpdated(DockItem *item)
{
    item->parentWidget()->adjustSize();
    resizeDockIcon();
}

void MainPanel::updateLayout()
{
    switch (m_settings->position()) {
    case DockSettings::Left:
//        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
//        m_appAreaLayout->setDirection(QBoxLayout::TopToBottom);
        m_appArea->layout()->setDirection(QBoxLayout::TopToBottom);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        break;
    case DockSettings::Bottom:
//        m_appAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // m_appArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
//        m_appAreaLayout->setDirection(QBoxLayout::LeftToRight);
        m_appArea->layout()->setDirection(QBoxLayout::LeftToRight);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        break;
    default:
        break;
    }

    resizeDockIcon();

    emit requestResized();
}

void MainPanel::resizeDockIcon()
{
    const int iconSize = m_settings->iconSize();
    // const int padding = 10;

    m_appArea->setRange(iconSize);
    m_appArea->setIconSize(iconSize);

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
