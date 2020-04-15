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
#include <QScrollBar>
#include <QScroller>
#include <QTimer>
#include <QDebug>

AppScrollArea::AppScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_mainWidget(new QWidget),
      m_scrollAni(new QVariantAnimation(this)),
      m_range(10)
{
    m_scrollAni->setDuration(500);
    m_scrollAni->setEasingCurve(QEasingCurve::InQuad);

    m_mainWidget->setLayout(m_mainLayout);

    setAttribute(Qt::WA_TranslucentBackground);
    setFrameShape(QFrame::NoFrame);
    setWidget(m_mainWidget);
    setWidgetResizable(true);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setStyleSheet("background: transparent;");
    setContentsMargins(0, 0, 0, 0);

    // TODO: Adjust the order
    QScroller *scroller = QScroller::scroller(this);
    scroller->grabGesture(this, QScroller::TouchGesture);
    scroller->grabGesture(this, QScroller::LeftMouseButtonGesture);
    connect(scroller, &QScroller::stateChanged, this, &AppScrollArea::onScrollerStateChanged);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_scrollAni, &QVariantAnimation::valueChanged, this, [=] (QVariant val) {
        if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
            horizontalScrollBar()->setValue(val.toInt());
        } else {
            verticalScrollBar()->setValue(val.toInt());
        }
    });
}

void AppScrollArea::addItem(AppItem *item)
{
    m_mainLayout->addWidget(item);
    m_mainWidget->adjustSize();

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
    m_scrollAni->stop();

    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        m_scrollAni->setStartValue(horizontalScrollBar()->value());
        m_scrollAni->setEndValue(item->geometry().x());
        m_scrollAni->start();
    } else {
        m_scrollAni->setStartValue(verticalScrollBar()->value());
        m_scrollAni->setEndValue(item->geometry().y());
        m_scrollAni->start();
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
    if (state == QScroller::Pressed) {
        for (int i = 0; i < m_mainLayout->count(); ++i) {
            static_cast<DockItem *>(m_mainLayout->itemAt(i)->widget())->hidePopup();
        }
    }
}

void AppScrollArea::wheelEvent(QWheelEvent *e)
{
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
         horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range));
    } else {
         verticalScrollBar()->setValue(verticalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range));
    }
    return;

    QScrollArea::wheelEvent(e);
}

void AppScrollArea::mouseMoveEvent(QMouseEvent *e)
{

    QScrollArea::mouseMoveEvent(e);
}
