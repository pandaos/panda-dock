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
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QScrollBar>
#include <QScroller>
#include <QTimer>
#include <QDebug>

AppScrollArea::AppScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_mainWidget(new QWidget),
      m_range(10)
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

    setFocusPolicy(Qt::StrongFocus);
    m_mainWidget->setFocusPolicy(Qt::StrongFocus);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AppScrollArea::addItem(AppItem *item)
{
    item->setParent(this);
    m_mainLayout->addWidget(item);
    m_mainWidget->adjustSize();

    connect(item, &AppItem::dragStarted, this, &AppScrollArea::itemDragStarted, Qt::UniqueConnection);

    // 初始化时不需要滚动到相应位置
    if (!item->entry()->WIdList.isEmpty()) {
        QTimer::singleShot(100, this, [=] { scrollToItem(item); });
    }
}

void AppScrollArea::removeItem(AppItem *item)
{
    m_mainLayout->removeWidget(item);
}

void AppScrollArea::scrollToItem(AppItem *item)
{
    QScroller::scroller(this)->stop();

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
    for (int i = 0; i < layout()->count(); ++i) {
        layout()->itemAt(i)->widget()->setFixedSize(size, size);
    }
}

void AppScrollArea::onScrollerStateChanged(QScroller::State state)
{
    m_dragging = false;

    if (state == QScroller::Pressed || state == QScroller::Inactive) {
        for (int i = 0; i < m_mainLayout->count(); ++i) {
            static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget())->hidePopup();
            static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget())->setBlockMouseRelease(state != QScroller::Inactive);
        }
    }
}

void AppScrollArea::itemDragStarted()
{
    m_draggingItem = qobject_cast<AppItem *>(sender());
}

AppItem *AppScrollArea::itemAt(const QPoint &point)
{
    for (int i = 0; i < m_mainLayout->count(); ++i) {
        AppItem *item = static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget());

        QRect rect;
        rect.setSize(item->size());
        rect.setTopLeft(mapToParent(item->pos()));

        if (rect.contains(point)) {
            return item;
        }
    }

    return nullptr;
}

void AppScrollArea::wheelEvent(QWheelEvent *e)
{
    e->ignore();

    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        QScroller::scroller(this)->scrollTo(QPointF(horizontalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range), 0));
    } else {
        QScroller::scroller(this)->scrollTo(QPointF(0, verticalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range * 2)));
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

    QPoint pos = e->pos();
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        pos += QPoint((horizontalScrollBar()->value() / m_draggingItem->rect().width()) * m_draggingItem->rect().width(), 0);
    } else {
        pos += QPoint(0, (verticalScrollBar()->value() / m_draggingItem->rect().height()) * m_draggingItem->rect().height());
    }

    AppItem *currentItem = itemAt(pos);

    if (!currentItem)
        return;

    if (e->source()) {
        if (currentItem == m_draggingItem)
            return;

        if (!m_draggingItem)
            return;

        const int moveIndex = layout()->indexOf(m_draggingItem);
        const int replaceIndex = layout()->indexOf(currentItem);

        layout()->removeWidget(m_draggingItem);
        layout()->insertWidget(replaceIndex, m_draggingItem);

        /* drop 之后接收不到 wheel 事件
         * 不知道为啥，把 scroller stop 之后问题就解决
         */
        QScroller::scroller(this)->stop();
    }
}

void AppScrollArea::dropEvent(QDropEvent *e)
{
    m_draggingItem = nullptr;
    m_dragging = false;
}
