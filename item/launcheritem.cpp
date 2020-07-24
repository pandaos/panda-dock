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

#include "launcheritem.h"
#include "utils/utils.h"
#include "utils/docksettings.h"
#include <QPainter>
#include <QProcess>
#include <QDebug>

LauncherItem::LauncherItem(QWidget *parent)
    : DockItem(parent)
{
    setAccessibleName("Launcher");
    refreshIcon();
}

QString LauncherItem::popupText()
{
    return tr("Launcher");
}

void LauncherItem::refreshIcon()
{
    const int iconSize = static_cast<int>(qMin(width(), height()) * 0.8);

    m_iconPixmap = Utils::getIcon("launcher", iconSize);

    QWidget::update();
}

void LauncherItem::paintEvent(QPaintEvent *e)
{
    DockItem::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    DockSettings::Position position = DockSettings::instance()->position();
    qreal offset = std::min(rect().width(), rect().height()) * 0.02;
    qreal offsetX = (position == DockSettings::Left) ? offset : 0.0;
    qreal offsetY = (position == DockSettings::Bottom) ? offset : 0.0;

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;

    painter.drawPixmap(iconX + offsetX, iconY - offsetY, m_iconPixmap);
}

void LauncherItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void LauncherItem::mouseReleaseEvent(QMouseEvent *e)
{
    DockItem::mouseReleaseEvent(e);

    QProcess::startDetached("panda-launcher", QStringList());
}
