/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
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

#include "themeappicon.h"

#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QPixmapCache>
#include <QCryptographicHash>
#include <KF5/KWindowSystem/KWindowSystem>

ThemeAppIcon::ThemeAppIcon(QObject *parent) : QObject(parent)
{

}

ThemeAppIcon::~ThemeAppIcon()
{

}

const QPixmap ThemeAppIcon::getIcon(const QString iconName, const quint64 winId, const int size, const qreal ratio)
{
    QPixmap pixmap;

    // 把size改为小于size的最大偶数 :)
    const int s = int(size * ratio) & ~1;

    // load pixmap from Icon-Theme.
    const QIcon icon = QIcon::fromTheme(iconName);
    const int fakeSize = std::max(48, s); // cannot use 16x16, cause 16x16 is label icon
    pixmap = icon.pixmap(QSize(fakeSize, fakeSize));

    if (pixmap.isNull() && winId != 0) {
        pixmap = KWindowSystem::icon(winId, size, size, true);
    }

    if (pixmap.isNull()) {
        pixmap = QIcon::fromTheme("application-x-desktop").pixmap(QSize(fakeSize, fakeSize));
    }

    if (!pixmap.isNull()) {
        if (pixmap.size().width() != s) {
            pixmap = pixmap.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        pixmap.setDevicePixelRatio(ratio);
    }

    return pixmap;
}
