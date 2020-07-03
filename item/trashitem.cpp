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

#include "trashitem.h"
#include "utils/utils.h"
#include "utils/docksettings.h"
#include <QMouseEvent>
#include <QProcess>
#include <QPainter>
#include <QDialog>
#include <QDir>

#include <QDBusConnection>
#include <QDBusInterface>

const QString TrashDir = QDir::homePath() + "/.local/share/Trash";
const QDir::Filters ItemsShouldCount = QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot;

TrashItem::TrashItem(QWidget *parent)
    : DockItem(parent),
      m_filesWatcher(new QFileSystemWatcher(this)),
      m_rightMenu(new QMenu(this))
{
    QAction *openAction = new QAction(tr("Open"));
    QAction *emptyAction = new QAction(tr("Empty"));

    m_rightMenu->addAction(openAction);
    m_rightMenu->addAction(emptyAction);

    connect(m_filesWatcher, &QFileSystemWatcher::directoryChanged, this, &TrashItem::onDirectoryChanged, Qt::QueuedConnection);
    connect(openAction, &QAction::triggered, this, &TrashItem::openTrashFold);
    connect(emptyAction, &QAction::triggered, this, &TrashItem::emptyTrash);

    setAccessibleName("Trash");
    onDirectoryChanged();
}

QString TrashItem::popupText()
{
    return tr("Trash");
}

void TrashItem::refreshIcon()
{
    const int iconSize = static_cast<int>(qMin(width(), height()) * 0.8);

    if (m_count) {
        m_iconPixmap = Utils::renderSVG(":/resources/user-trash-full.svg", QSize(iconSize, iconSize));
    } else {
        m_iconPixmap = Utils::renderSVG(":/resources/user-trash.svg", QSize(iconSize, iconSize));
    }

    QWidget::update();
}

void TrashItem::onDirectoryChanged()
{
    m_filesWatcher->addPath(TrashDir);

    if (QDir(TrashDir + "/files").exists()) {
        m_filesWatcher->addPath(TrashDir + "/files");
        m_count = QDir(TrashDir + "/files").entryList(ItemsShouldCount).count();
    } else {
        m_count = 0;
    }

    refreshIcon();
}

void TrashItem::openTrashFold()
{
    QProcess::startDetached("gio", QStringList() << "open" << "trash:///");
}

void TrashItem::emptyTrash()
{
    if (m_count) {
        QDBusConnection dbus = QDBusConnection::sessionBus();
        QDBusInterface iface(QLatin1String("org.panda.files"), QStringLiteral("/Files"), QLatin1String("org.panda.Files"), dbus, this);
        iface.call(QStringLiteral("emptyTrash"));
    }
}

void TrashItem::paintEvent(QPaintEvent *e)
{
    DockItem::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const qreal roundSize = std::min(rect().width(), rect().height()) * 0.06;
    DockSettings::Position position = DockSettings::instance()->position();
    qreal offset = std::min(rect().width(), rect().height()) * 0.02;
    qreal offsetX = (position == DockSettings::Left) ? offset : 0.0;
    qreal offsetY = (position == DockSettings::Bottom) ? offset : 0.0;

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;

    painter.drawPixmap(iconX + offsetX, iconY - offsetY, m_iconPixmap);
}

void TrashItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void TrashItem::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        m_rightMenu->move(QCursor::pos());
        m_rightMenu->exec();
    }
}

void TrashItem::mouseReleaseEvent(QMouseEvent *e)
{
    openTrashFold();
    QWidget::mouseReleaseEvent(e);
}
