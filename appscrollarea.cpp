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
#include <QDebug>

AppScrollArea::AppScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_mainWidget(new QWidget),
      m_range(10)
{
    m_mainWidget->setLayout(m_mainLayout);

    setAttribute(Qt::WA_TranslucentBackground);
    setFrameShape(QFrame::NoFrame);
    setWidget(m_mainWidget);
    setWidgetResizable(true);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
//    m_mainWidget->setAutoFillBackground(false);

    setStyleSheet("background: transparent;");

    setContentsMargins(0, 0, 0, 0);

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
}

void AppScrollArea::removeItem(AppItem *item)
{
    m_mainLayout->removeWidget(item);
}

void AppScrollArea::scrollToItem(AppItem *item)
{
    return;
    blockSignals(true);
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        horizontalScrollBar()->setValue(item->geometry().x());
    } else {
        verticalScrollBar()->setValue(item->geometry().y());
    }
    blockSignals(false);
}

void AppScrollArea::setRange(int value)
{
    m_range = value;
}

void AppScrollArea::onScrollerStateChanged(QScroller::State state)
{
    if (state == QScroller::Pressed) {
        for (int i = 0; i < m_mainLayout->count(); ++i) {
            static_cast<AppItem *>(m_mainLayout->itemAt(i)->widget())->hideTips();
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
