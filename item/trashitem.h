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

#ifndef TRASHITEM_H
#define TRASHITEM_H

#include "dockitem.h"
#include <QFileSystemWatcher>

class TrashItem : public DockItem
{
    Q_OBJECT

public:
    explicit TrashItem(QWidget *parent = nullptr);

    inline ItemType itemType() const override { return DockItem::Fixed; }

private:
    void refreshIcon();
    void onDirectoryChanged();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QPixmap m_iconPixmap;
    QFileSystemWatcher *m_filesWatcher;
    int m_count = 0;
};

#endif // TRASHITEM_H
