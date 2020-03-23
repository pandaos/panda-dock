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

#include "showdesktopitem.h"
#include "utils/utils.h"
#include <QPainter>
#include <KF5/KWindowSystem/KWindowSystem>

ShowDesktopItem::ShowDesktopItem(QWidget *parent)
    : DockItem(parent)
{
    setAccessibleName("ShowDesktop");
    refreshIcon();
}

void ShowDesktopItem::refreshIcon()
{
    const int iconSize = qMin(width(), height());

    m_iconPixmap = Utils::renderSVG(":/resources/desktop.svg", QSize(iconSize * 0.8, iconSize * 0.8));

    QWidget::update();
}

void ShowDesktopItem::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;

    painter.drawPixmap(iconX, iconY, m_iconPixmap);
}

void ShowDesktopItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void ShowDesktopItem::mouseReleaseEvent(QMouseEvent *e)
{
    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());

    QWidget::mouseReleaseEvent(e);
}
