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

#include "blurwindow.h"
#include <QPainter>
#include <KWindowEffects>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>

BlurWindow::BlurWindow(QWidget *parent)
    : QWidget(parent),
      m_textLabel(new QLabel)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_textLabel);
    setLayout(layout);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setVisible(false);
}

void BlurWindow::setText(const QString &text)
{
    m_textLabel->setText(text);
}

void BlurWindow::update()
{
    QWidget::update();
}

void BlurWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(QColor(0, 0, 0, 20));
    painter.setBrush(QColor(241, 241, 241, 200));
    const qreal radius = std::min(rect().height(), rect().width()) / 2 - 2;
    painter.drawRoundedRect(rect(), radius, radius);
}

void BlurWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    const qreal radius = std::min(rect().height(), rect().width()) / 2 - 2;
    QPainterPath path;
    path.addRoundedRect(this->rect(), radius, radius);
    KWindowEffects::enableBlurBehind(winId(), true, path.toFillPolygon().toPolygon());
}

void BlurWindow::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    KWindowEffects::enableBlurBehind(winId(), false);
}
