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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "mainpanel.h"
#include "utils/dockitemmanager.h"
#include "utils/docksettings.h"

class EventMonitor;
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateSize();

private:
    void initSlideWindow();
    void updateStrutPartial();
    void onPositionChanged();
    void onHideModeChanged();
    void onMouseMove(int x, int y);
    void updateBlurRegion();
    void delayUpdateBlurRegion();

    QPainterPath getCornerPath();

    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    MainPanel *m_mainPanel;
    DockItemManager *m_itemManager;
    DockSettings *m_settings;
    //QWidget *m_fakeWidget;
    EventMonitor *m_eventMonitor;

    int m_frameRadius = 9;
};

#endif // MAINWINDOW_H
