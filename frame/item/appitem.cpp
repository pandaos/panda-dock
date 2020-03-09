#include "appitem.h"
#include "utils/utils.h"
#include "utils/themeappicon.h"
#include "controller/appwindowmanager.h"
#include "xcb/xcbmisc.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QToolTip>

AppItem::AppItem(DockEntry *entry, QWidget *parent)
    : DockItem(parent),
      m_entry(entry),
      m_updateIconGeometryTimer(new QTimer(this)),
      m_openAction(new QAction(tr("Open"))),
      m_dockAction(new QAction(""))
{
    m_updateIconGeometryTimer->setInterval(500);
    m_updateIconGeometryTimer->setSingleShot(true);

    QAction *closeAction = new QAction("Close All");
    m_contextMenu.addAction(m_openAction);
    m_contextMenu.addAction(m_dockAction);
    m_contextMenu.addAction(closeAction);

    initDockAction();
    refreshIcon();

    connect(m_updateIconGeometryTimer, &QTimer::timeout, this, &AppItem::updateWindowIconGeometries, Qt::QueuedConnection);
    connect(closeAction, &QAction::triggered, this, &AppItem::closeWindow);
    connect(m_dockAction, &QAction::triggered, this, &AppItem::dockActionTriggered);
    connect(m_openAction, &QAction::triggered, this, [=] {
        AppWindowManager::instance()->openApp(m_entry->icon);
    });
}

void AppItem::closeWindow()
{
    for (quint64 id : m_entry->WIdList) {
        AppWindowManager::instance()->closeWindow(id);
    }
}

void AppItem::update()
{
    initDockAction();
    refreshIcon();
    QWidget::update();
}

void AppItem::initDockAction()
{
    m_openAction->setVisible(m_entry->WIdList.isEmpty());

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

void AppItem::refreshIcon()
{
    const int iconSize = qMin(width(), height());
    const QString iconName = m_entry->icon;

    if (m_entry->WIdList.isEmpty()) {
        m_iconPixmap = ThemeAppIcon::getIcon(iconName, 0, iconSize * 0.8, devicePixelRatioF());
    } else {
        m_iconPixmap = ThemeAppIcon::getIcon(iconName, m_entry->WIdList.at(m_entry->current), iconSize * 0.8, devicePixelRatioF());
    }

    QWidget::update();

    m_updateIconGeometryTimer->start();
}

void AppItem::updateWindowIconGeometries()
{
    const QRect r(mapToGlobal(QPoint(0, 0)),
                  mapToGlobal(QPoint(width(), height())));
    auto *xcb_misc = XcbMisc::instance();

    for (quint64 id : m_entry->WIdList) {
        xcb_misc->set_window_icon_geometry(id, r);
    }
}

void AppItem::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(Qt::NoPen);

    // draw background
    const QRectF itemRect = rect();
//    qreal min = qMin(itemRect.width(), itemRect.height());
//    QRectF backgroundRect = QRectF(itemRect.x(), itemRect.y(), min, min);
//    backgroundRect = backgroundRect.marginsRemoved(QMargins(3, 3, 3, 3));
//    backgroundRect.moveCenter(itemRect.center());
//    QPainterPath path;
//    path.addRoundedRect(backgroundRect, 8, 8);
//    if (m_entry->isActive) {
//        painter.fillPath(path, QColor(0, 0, 0, 255 * 0.8));
//    } else {
//        painter.fillPath(path, QColor(0, 0, 0, 255 * 0.3));
//    }

    const int lineWidth = itemRect.width() / 2;
    const int lineHeight = 2;

    if (!m_entry->WIdList.isEmpty()) {
        if (m_entry->isActive) {
            painter.setBrush(QColor("#1974FF"));
            painter.drawRect(QRect((itemRect.width() - lineWidth) / 2, itemRect.height() - lineHeight - 1, lineWidth, lineHeight));
        } else {
            painter.setBrush(QColor("#0F0F0F"));
            painter.drawRect(QRect((itemRect.width() - lineWidth) / 2, itemRect.height() - lineHeight - 1, lineWidth, lineHeight));
        }
    }

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;
    painter.drawPixmap(iconX, iconY, m_iconPixmap);
}

void AppItem::mousePressEvent(QMouseEvent *e)
{
    m_updateIconGeometryTimer->stop();

    if (e->button() == Qt::RightButton) {
        m_contextMenu.popup(QCursor::pos());
    }

    DockItem::mousePressEvent(e);
}

void AppItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        AppWindowManager::instance()->clicked(m_entry);
    }
}

void AppItem::resizeEvent(QResizeEvent *e)
{
    DockItem::resizeEvent(e);

    refreshIcon();
}
