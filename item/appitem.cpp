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

#include "appitem.h"
#include "utils/utils.h"
#include "utils/appwindowmanager.h"
#include "utils/docksettings.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDebug>
#include <QToolTip>
#include <QX11Info>
#include <QTimer>
#include <KWindowSystem>

AppItem::AppItem(DockEntry *entry, QWidget *parent)
    : DockItem(parent),
      m_entry(entry),
      m_updateIconGeometryTimer(new QTimer(this)),
      m_dragActiveTimer(new QTimer(this)),
      m_openAction(new QAction(tr("Open"))),
      m_closeAction(new QAction(tr("Close All"))),
      m_dockAction(new QAction(""))
{
    m_updateIconGeometryTimer->setInterval(200);
    m_updateIconGeometryTimer->setSingleShot(true);

    m_dragActiveTimer->setInterval(350);
    m_dragActiveTimer->setSingleShot(true);

    m_contextMenu.addAction(m_openAction);
    m_contextMenu.addAction(m_dockAction);
    m_contextMenu.addAction(m_closeAction);

    setAcceptDrops(true);

    initDockAction();
    initStates();
    updateIcon();

    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AppItem::initStates);
    connect(m_updateIconGeometryTimer, &QTimer::timeout, this, &AppItem::updateIconGeometry, Qt::QueuedConnection);
    connect(m_dragActiveTimer, &QTimer::timeout, this, &AppItem::startDrag);
    connect(m_openAction, &QAction::triggered, this, &AppItem::open);
    connect(m_closeAction, &QAction::triggered, this, &AppItem::close);
    connect(m_dockAction, &QAction::triggered, this, &AppItem::dockActionTriggered);
}

AppItem::~AppItem()
{
}

QString AppItem::popupText()
{
    return m_entry->visibleName;
}

DockEntry *AppItem::entry()
{
    return m_entry;
}

void AppItem::open()
{
    AppWindowManager::instance()->openApp(m_entry->className);
}

void AppItem::close()
{
    for (quint64 id : m_entry->WIdList) {
        AppWindowManager::instance()->closeWindow(id);
    }
}

void AppItem::update()
{
    initDockAction();
    updateIcon();

    QWidget::update();
}

void AppItem::setBlockMouseRelease(bool enable)
{
    m_blockMouseRelease = enable;
}

void AppItem::initDockAction()
{
    m_openAction->setVisible(m_entry->WIdList.isEmpty());
    m_closeAction->setVisible(!m_entry->WIdList.isEmpty());

    if (m_entry->isDocked) {
        m_dockAction->setText(tr("UnDock"));
    } else {
        m_dockAction->setText(tr("Dock"));
    }
}

void AppItem::dockActionTriggered()
{
    if (m_entry->isDocked) {
        AppWindowManager::instance()->undock(m_entry);
    }

    m_entry->isDocked = !m_entry->isDocked;

    initDockAction();
    AppWindowManager::instance()->save();
}

void AppItem::updateIcon()
{
    const qreal iconSize = qMin(width(), height());
    const QString iconName = m_entry->iconName;

    m_iconPixmap = Utils::getIcon(iconName, iconSize * 0.7);

    QWidget::update();

    m_updateIconGeometryTimer->start();
}

void AppItem::updateIconGeometry()
{
    QRect rect = geometry();
    rect.moveTo(mapToGlobal(QPoint(0, 0)));

    for (quint64 id : m_entry->WIdList) {
         NETWinInfo info(QX11Info::connection(),
                         id,
                         (WId) QX11Info::appRootWindow(),
                         NET::WMIconGeometry,
                         0);
         NETRect nrect;
         nrect.pos.x = rect.x();
         nrect.pos.y = rect.y();
         nrect.size.height = rect.height();
         nrect.size.width = rect.width();
         info.setIconGeometry(nrect);
    }
}

void AppItem::initStates()
{
    m_entry->isActive = m_entry->WIdList.contains(KWindowSystem::activeWindow());

    update();
}

void AppItem::startDrag()
{
    m_dragging = true;
    update();

    QPixmap pixmap = m_iconPixmap;
    m_drag = new QDrag(this);
    m_drag->setMimeData(new QMimeData);

    connect(m_drag, &QDrag::destroyed, this, [=] {
        m_dragging = false;
        update();
    });

    m_drag->QDrag::setPixmap(pixmap);
    m_drag->setHotSpot(pixmap.rect().center() / pixmap.devicePixelRatioF());
    emit dragStarted();
    m_drag->exec(Qt::MoveAction);
}

void AppItem::paintEvent(QPaintEvent *e)
{
    if (m_dragging)
        return;

    DockItem::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(Qt::NoPen);

    // draw background
    const QRectF itemRect = rect();
    const int lineWidth = itemRect.width() * 0.3;
    const int lineHeight = 2;

    if (!m_entry->WIdList.isEmpty()) {
        // QPixmap pixmap(":/resources/dot-normal.svg");
        // QPoint p;
        // p.setX((itemRect.width() - pixmap.width()) / 2);
        // p.setY((itemRect.height() - pixmap.height()) - 4);
        // painter.drawPixmap(p, pixmap);

        QRect activeRect;
        if (DockSettings::instance()->position() == DockSettings::Left) {
            activeRect = QRect(1,
                               (itemRect.height() - lineWidth) / 2,
                               lineHeight, lineWidth);
        } else {
            activeRect = QRect((itemRect.width() - lineWidth) / 2,
                               itemRect.height() - lineHeight - 2,
                               lineWidth, lineHeight);
        }

        if (m_entry->isActive) {
            painter.setBrush(QColor("#1974FF"));
            painter.drawRect(activeRect);
        } else {
            painter.setBrush(QColor("#0F0F0F"));
            painter.drawRect(activeRect);
        }
    }

    const auto ratio = devicePixelRatioF();
    const qreal iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const qreal iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;
    painter.drawPixmap(iconX, iconY, m_iconPixmap);
}

void AppItem::mousePressEvent(QMouseEvent *e)
{
    DockItem::hidePopup();

    m_updateIconGeometryTimer->stop();
    m_dragActiveTimer->stop();

    if (e->button() == Qt::LeftButton) {
        m_mousePressPos = e->pos();
        m_dragActiveTimer->start();
    }

    if (e->button() == Qt::RightButton) {
        m_contextMenu.popup(QCursor::pos());
        return;
    }

    DockItem::mousePressEvent(e);
}

void AppItem::mouseMoveEvent(QMouseEvent *e)
{
    e->ignore();
}

void AppItem::mouseReleaseEvent(QMouseEvent *e)
{
    m_dragActiveTimer->stop();

    if (m_blockMouseRelease)
        return;

    if (e->button() == Qt::LeftButton) {
        AppWindowManager::instance()->clicked(m_entry);
    }

    DockItem::mouseReleaseEvent(e);
}

void AppItem::resizeEvent(QResizeEvent *e)
{
    DockItem::resizeEvent(e);

    updateIcon();
}

void AppItem::enterEvent(QEvent *e)
{
    DockItem::enterEvent(e);
}

void AppItem::leaveEvent(QEvent *e)
{
    DockItem::leaveEvent(e);
}
