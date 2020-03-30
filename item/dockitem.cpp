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

#include "../utils/docksettings.h"
#include "dockitem.h"
#include <QMouseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QTimer>

DockItem::DockItem(QWidget *parent)
    : QWidget(parent),
      m_hoverAnimation(new QVariantAnimation(this)),
      m_popupDelayTimer(new QTimer(this)),
      m_hoverSize(0),
      m_popupWidget(new BlurWindow)
{
    m_hoverAnimation->setDuration(250);
    m_popupDelayTimer->setInterval(150);
    m_popupDelayTimer->setSingleShot(true);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_hoverAnimation, &QVariantAnimation::valueChanged, this, [=] (const QVariant &value) {
        m_hoverSize = value.toReal();
        QWidget::update();
    });

    connect(m_popupDelayTimer, &QTimer::timeout, this, &DockItem::showPopup);
}

QSize DockItem::sizeHint() const
{
    int size = qMin(maximumWidth(), maximumHeight());

    return QSize(size, size);
}

const QString DockItem::contextMenu() const
{
    return QString();
}

const QRect DockItem::iconRect() const
{
    const QRect itemRect = rect();
    QRect iconRect;

    const int iconSize = std::min(itemRect.width(), itemRect.height()) * 0.8;
    iconRect.setWidth(iconSize);
    iconRect.setHeight(iconSize);

    iconRect.moveTopLeft(itemRect.center() - iconRect.center());
    return iconRect;
}

void DockItem::showPopup()
{
    if (popupText().isEmpty())
        return;

    QPoint p = mapToGlobal(QPoint(0, 0));

    m_popupWidget->setText(popupText());
    m_popupWidget->setVisible(true);

    if (DockSettings::instance()->position() == DockSettings::Bottom)
        p += QPoint(width() / 2 - m_popupWidget->width() / 2, 0);
    else
        p += QPoint(0, height() / 2 - m_popupWidget->height() / 2);

    m_popupWidget->move(p);
    m_popupWidget->update();
}

void DockItem::hidePopup()
{
    m_popupWidget->hide();
}

void DockItem::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (m_hoverSize) {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 30));
        QRect roundRect = QRect(0, 0, m_hoverSize, m_hoverSize);
        roundRect.moveCenter(rect().center());
        int radius = m_hoverSize * 0.4;
        painter.drawRoundRect(roundRect, radius, radius);
    }
}

void DockItem::enterEvent(QEvent *e)
{
    // Remove the bottom area to prevent unintentional operation in auto-hide mode.
//    if (!rect().adjusted(0, 0, width(), height() - 5).contains(mapFromGlobal(QCursor::pos()))) {
//        return;
//    }

    m_hoverAnimation->stop();
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutBack);
    m_hoverAnimation->setStartValue(0.0);
    m_hoverAnimation->setEndValue(rect().width() * 0.8);
    m_hoverAnimation->start();

    m_popupDelayTimer->start();

    QWidget::update();
    QWidget::enterEvent(e);
}

void DockItem::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    hidePopup();
    m_popupDelayTimer->stop();
    m_hoverAnimation->stop();
    m_hoverAnimation->setEasingCurve(QEasingCurve::InBack);
    m_hoverAnimation->setStartValue(m_hoverSize);
    m_hoverAnimation->setEndValue(0.0);
    m_hoverAnimation->start();

    QWidget::update();
}

void DockItem::mousePressEvent(QMouseEvent *e)
{
    hidePopup();

//    if (e->button() == Qt::RightButton) {
//        if (iconRect().contains(e->pos())) {
//            showContextMenu();
//        }
//    }

    QWidget::mousePressEvent(e);
}

void DockItem::mouseReleaseEvent(QMouseEvent *e)
{
    hidePopup();

    QWidget::mouseReleaseEvent(e);
}
