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

#ifndef BLURWINDOW_H
#define BLURWINDOW_H

#include <QWidget>

class QLabel;
class BlurWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlurWindow(QWidget *parent = nullptr);

    void setText(const QString &text);
    void update();

private:
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    QLabel *m_textLabel;
};

#endif // BLURWINDOW_H
