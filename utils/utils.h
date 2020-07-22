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

#include <QPixmap>
#include <QImageReader>
#include <QApplication>
#include <QScreen>
#include <QIcon>
#include <QFile>

namespace Utils
{
    static QPixmap renderSVG(const QString &path, const QSize &size) {
        const qreal devicePixelRatio = qApp->devicePixelRatio();
        QImageReader reader;
        QPixmap pixmap;
        reader.setFileName(path);
        if (reader.canRead()) {
            reader.setScaledSize(size * devicePixelRatio);
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
        else {
            pixmap.load(path);
        }

        return pixmap;
    }

    static const QPixmap getIcon(const QString iconName, const qreal size)
    {
        const qreal ratio = qApp->devicePixelRatio();
        QPixmap pixmap;

        // 文件路径处理
        if (QFile::exists(iconName)) {
            if (iconName.endsWith(".svg"))
                pixmap = renderSVG(iconName, QSize(size * ratio, size * ratio));
            else
                pixmap = QPixmap(iconName).scaled(size * ratio, size * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        // 从 theme 中获取
        if (pixmap.isNull()) {
            // 把 size 改为小于size的最大偶数
            const int s = int(size * ratio) & ~1;
            const int fakeSize = std::max(48, s); // cannot use 16x16, cause 16x16 is label icon
            const QIcon icon = QIcon::fromTheme(iconName, QIcon::fromTheme("application-x-desktop"));
            pixmap = icon.pixmap(QSize(fakeSize, fakeSize));

            // 如果找不到需要返回空的pixmap
            /* const QIcon icon = QIcon::fromTheme(iconName, QIcon()); */
            /* if (!icon.isNull()) { */
            /*     pixmap = icon.pixmap(QSize(fakeSize, fakeSize)); */
            /* } */
        }

        pixmap.setDevicePixelRatio(ratio);

        return pixmap;
    }

}
