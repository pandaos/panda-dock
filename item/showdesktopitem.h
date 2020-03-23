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

#ifndef SHOWDESKTOPITEM_H
#define SHOWDESKTOPITEM_H

#include "dockitem.h"

class ShowDesktopItem : public DockItem
{
    Q_OBJECT

public:
    explicit ShowDesktopItem(QWidget *parent = nullptr);

    inline virtual ItemType itemType() const { return Fixed; }

private:
    void refreshIcon();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QPixmap m_iconPixmap;
};

#endif // SHOWDESKTOPITEM_H
