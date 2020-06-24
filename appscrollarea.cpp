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

#include "appscrollarea.h"
#include "utils/appwindowmanager.h"
#include <QApplication>
#include <QDragEnterEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QScroller>
#include <QTimer>
#include <QDebug>

AppScrollArea::AppScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_draggingItem(nullptr),
      m_mainWidget(new QWidget),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_range(10),
      m_iconSize(0)
{
    m_mainWidget->setLayout(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");
    setFrameShape(QFrame::NoFrame);
    setContentsMargins(0, 0, 0, 0);
    setWidget(m_mainWidget);
    setWidgetResizable(true);
    setAcceptDrops(true);
    setMouseTracking(true);

    // TODO: Adjust the order
    QScroller *scroller = QScroller::scroller(this);
    scroller->grabGesture(this, QScroller::TouchGesture);
    scroller->grabGesture(this, QScroller::LeftMouseButtonGesture);
    connect(scroller, &QScroller::stateChanged, this, &AppScrollArea::onScrollerStateChanged);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AppScrollArea::addItem(AppItem *item)
{
    m_mainLayout->addWidget(item);
    m_mainWidget->adjustSize();

    connect(item, &AppItem::dragStarted, this, &AppScrollArea::itemDragStarted, Qt::UniqueConnection);

    // 初始化时不需要滚动到相应位置
    if (!item->entry()->WIdList.isEmpty()) {
        QTimer::singleShot(0, this, [=] { scrollToItem(item); });
    }
}

void AppScrollArea::removeItem(AppItem *item)
{
    m_mainLayout->removeWidget(item);
}

void AppScrollArea::scrollToItem(AppItem *item)
{
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        QScroller::scroller(this)->scrollTo(QPointF(item->geometry().x(), 0));
    } else {
        QScroller::scroller(this)->scrollTo(QPointF(0, item->geometry().y()));
    }
}

void AppScrollArea::setRange(int value)
{
    m_range = value;
}

void AppScrollArea::setIconSize(int size)
{
    m_iconSize = size;

    for (int i = 0; i < layout()->count(); ++i) {
        if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
            layout()->itemAt(i)->widget()->setFixedSize(size, QWIDGETSIZE_MAX);
        } else {
            layout()->itemAt(i)->widget()->setFixedSize(QWIDGETSIZE_MAX, size);
        }
    }
}

void AppScrollArea::onScrollerStateChanged(QScroller::State state)
{
    m_dragging = false;

    for (int i = 0; i < m_mainLayout->count(); ++i) {
        AppItem *item = static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget());

        switch (state) {
        case QScroller::Pressed:
            item->hidePopup();
            item->setBlockMouseRelease(true);
            break;
        case QScroller::Inactive: {
            item->setBlockMouseRelease(false);
            break;
        }
        default:
            break;
        }
    }
}

void AppScrollArea::itemDragStarted()
{
    m_draggingItem = qobject_cast<AppItem *>(sender());

    // 开始的时候也要 stop，否则也会影响到 wheel event.
    QScroller::scroller(this)->stop();
}

AppItem *AppScrollArea::itemAt(const QPoint &point)
{
    QPoint pos = point;
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        pos += QPoint((horizontalScrollBar()->value() / m_iconSize) * m_iconSize, 0);
    } else {
        pos += QPoint(0, (verticalScrollBar()->value() / m_iconSize) * m_iconSize);
    }

    for (int i = 0; i < m_mainLayout->count(); ++i) {
        AppItem *item = static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget());
        QRect rect;
        rect.setSize(item->size());
        rect.setTopLeft(mapToParent(item->pos()));

        if (rect.contains(pos)) {
            return item;
        }
    }

    return static_cast<AppItem *>(m_mainLayout->itemAt(0)->widget());
    // return nullptr;
}

void AppScrollArea::wheelEvent(QWheelEvent *e)
{
    const int offset = -e->angleDelta().y() / 120;

    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + offset * m_range);
    } else {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + offset * m_range);
    }
}

void AppScrollArea::dragEnterEvent(QDragEnterEvent *e)
{
    AppItem *dragSourceItem = qobject_cast<AppItem *>(e->source());

    if (dragSourceItem) {
        e->accept();
    } else {
        m_draggingItem = nullptr;
    }
}

void AppScrollArea::mouseMoveEvent(QMouseEvent *e)
{
    if (m_dragging)
        return e->ignore();

    QScrollArea::mouseMoveEvent(e);
}

void AppScrollArea::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();

    AppItem *currentItem = itemAt(e->pos());

    if (!currentItem)
        return;

    if (e->source()) {
        if (currentItem == m_draggingItem)
            return;

        if (!m_draggingItem)
            return;

        const int fromIndex = layout()->indexOf(m_draggingItem);
        const int toIndex = layout()->indexOf(currentItem);

        layout()->removeWidget(m_draggingItem);
        layout()->insertWidget(toIndex, m_draggingItem);

        // 保存配置
        AppWindowManager::instance()->move(fromIndex, toIndex);
    }
}

void AppScrollArea::dropEvent(QDropEvent *e)
{
    Q_UNUSED(e)

    m_draggingItem = nullptr;
    m_dragging = false;

    /* drop 之后接收不到 wheel 事件
     * 不知道为啥，把 scroller stop 之后问题就解决
     */
    QScroller::scroller(this)->stop();
}
