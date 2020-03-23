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

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initWindowSize();

private:
    void setStrutPartial();
    void onPositionChanged();

    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    MainPanel *m_mainPanel;
    DockItemManager *m_itemManager;
    DockSettings *m_settings;
//    XcbMisc *m_xcbMisc;
    QWidget *m_fakeWidget;
    QSize m_size;
};

#endif // MAINWINDOW_H