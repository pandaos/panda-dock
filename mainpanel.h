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

#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QBoxLayout>
#include "appscrollarea.h"
#include "item/dockitem.h"
#include "utils/docksettings.h"
#include "utils/dockitemmanager.h"

class LauncherItem;
class TrashItem;
class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

signals:
    void requestResized();

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

private:
    void init();
    void updateLayout();
    void resizeDockIcon();
    void onPositionChanged();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QBoxLayout *m_mainLayout;
    AppScrollArea *m_appArea;
    LauncherItem *m_launcherItem;
    TrashItem *m_trashItem;

    DockItemManager *m_dockItemmanager;
    DockSettings *m_settings;
};

#endif // MAINPANEL_H
