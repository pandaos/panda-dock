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

#ifndef DOCKITEM_H
#define DOCKITEM_H

#include <QWidget>
#include <QPointer>
#include <QMenu>
#include <QVariantAnimation>

class DockItem : public QWidget
{
    Q_OBJECT

public:
    enum ItemType {
        Fixed,
        App
    };

    explicit DockItem(QWidget *parent = nullptr);

    inline virtual ItemType itemType() const { Q_UNREACHABLE(); return App; }

    QSize sizeHint() const override;

    virtual const QString contextMenu() const;
    const QRect perfectIconRect() const;

    virtual QWidget *popupTips();

private:
    void showContextMenu();
    const QPoint popupMarkPoint() const;
    const QPoint topleftPoint() const;

protected:
    void paintEvent(QPaintEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QVariantAnimation *m_hoverAnimation;
    QTimer *m_popupTipsDelayTimer;
    QMenu m_contextMenu;
    qreal m_hoverSize;
};

#endif // DOCKITEM_H
