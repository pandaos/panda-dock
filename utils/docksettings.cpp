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

#include "docksettings.h"
#include <QStandardPaths>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QDebug>
#include <math.h>

#define TOPBARHEIGHT 35

DockSettings *DockSettings::instance()
{
    static DockSettings instance;

    return &instance;
}

DockSettings::DockSettings(QObject *parent)
    : QObject(parent),
      m_settings(new QSettings(QString("%1/%2/%3/config.conf")
                               .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                               .arg(qApp->organizationName())
                               .arg(qApp->applicationName()), QSettings::IniFormat)),
      m_settingsMenu(new QMenu),
      m_leftPosAction(new QAction(tr("Left"), this)),
      m_bottomPosAction(new QAction(tr("Bottom"), this)),
      m_smallSizeAction(new QAction(tr("Small"), this)),
      m_mediumSizeAction(new QAction(tr("Medium"), this)),
      m_largeSizeAction(new QAction(tr("Large"), this))
{
    qDebug() << QString("%1/%2/%3/config.conf")
                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                .arg(qApp->organizationName())
                .arg(qApp->applicationName());

    if (!m_settings->contains("icon_size")) {
        m_settings->setValue("icon_size", 64);
    }

    if (!m_settings->contains("position")) {
        m_settings->setValue("position", Bottom);
    }

    m_position = static_cast<Position>(m_settings->value("position").toInt());

    m_leftPosAction->setCheckable(true);
    m_bottomPosAction->setCheckable(true);

    m_smallSizeAction->setCheckable(true);
    m_mediumSizeAction->setCheckable(true);
    m_largeSizeAction->setCheckable(true);

    QMenu *sizeSubMenu = new QMenu(m_settingsMenu);
    sizeSubMenu->addAction(m_smallSizeAction);
    sizeSubMenu->addAction(m_mediumSizeAction);
    sizeSubMenu->addAction(m_largeSizeAction);

    QAction *sizeSubAction = new QAction(tr("Size"), this);
    sizeSubAction->setMenu(sizeSubMenu);
    m_settingsMenu->addAction(sizeSubAction);

    initSizeAction();

    connect(m_smallSizeAction, &QAction::triggered, this, [=] {
        setIconSize(32);
    });

    connect(m_mediumSizeAction, &QAction::triggered, this, [=] {
        setIconSize(64);
    });

    connect(m_largeSizeAction, &QAction::triggered, this, [=] {
        setIconSize(96);
    });

    // ****************

    QMenu *positionSubMenu = new QMenu(m_settingsMenu);
    positionSubMenu->addAction(m_leftPosAction);
    positionSubMenu->addAction(m_bottomPosAction);

    QAction *positionSubAction = new QAction(tr("Position"), this);
    positionSubAction->setMenu(positionSubMenu);

    m_settingsMenu->addAction(positionSubAction);

    connect(m_leftPosAction, &QAction::triggered, this, [=] {
        m_position = Left;
        setValue("position", Left);
        emit positionChanged();
    });

    connect(m_bottomPosAction, &QAction::triggered, this, [=] {
        m_position = Bottom;
        setValue("position", Bottom);
        emit positionChanged();
    });
}

QRect DockSettings::primaryRawRect()
{
    return qApp->primaryScreen()->geometry();
}

const QRect DockSettings::windowRect() const
{
    QRect primaryRect = qApp->primaryScreen()->geometry();
    qreal scale = qApp->primaryScreen()->devicePixelRatio();
    const int iconSize = m_settings->value("icon_size").toInt();
    const int iconCount = DockItemManager::instance()->itemList().count() - 1;
    const int margin = 14;
    QSize panelSize;
    QPoint p;

    if (m_position == Bottom) {
        int maxWidth = primaryRect.width() - margin * 2;
        int calcWidth = iconSize * 2 + PADDING;
        for (int i = 1; i <= iconCount; ++i) {
            calcWidth += iconSize;

            if (calcWidth >= maxWidth) {
                calcWidth -= iconSize;
                break;
            }
        }
        panelSize.setHeight(iconSize);
        panelSize.setWidth(calcWidth);
    } else {
        primaryRect.setHeight(primaryRect.height() - TOPBARHEIGHT);
        primaryRect.setY(TOPBARHEIGHT);

        int maxHeight = primaryRect.height() - margin * 2;
        int calcHeight = iconSize * 2 + PADDING;
        for (int i = 1; i <= iconCount; ++i) {
            calcHeight += iconSize;

            if (calcHeight >= maxHeight) {
                calcHeight -= iconSize;
                break;
            }
        }
        panelSize.setHeight(calcHeight);
        panelSize.setWidth(iconSize);
    }

    const int offsetX = (primaryRect.width() - panelSize.width()) / 2;
    const int offsetY = (primaryRect.height() - panelSize.height()) / 2;
    if (m_position == Bottom) {
        p = QPoint(offsetX, primaryRect.height() - panelSize.height() - margin);
    } else {
        p = QPoint(margin, offsetY + TOPBARHEIGHT / 2);
    }

    return QRect(primaryRect.topLeft() + p, panelSize);
}

void DockSettings::setValue(const QString &key, const QVariant &variant)
{
    m_settings->setValue(key, variant);
}

void DockSettings::setIconSize(int size)
{
    m_settings->setValue("icon_size", size);
    initSizeAction();

    emit iconSizeChanged();
}

int DockSettings::iconSize() const
{
    return m_settings->value("icon_size").toInt();
}

void DockSettings::showSettingsMenu()
{
    m_leftPosAction->setChecked(m_position == Left);
    m_bottomPosAction->setChecked(m_position == Bottom);

    m_settingsMenu->exec(QCursor::pos());
}

void DockSettings::initSizeAction()
{
    const int size = iconSize();

    if (size <= 32) {
        m_smallSizeAction->setChecked(true);
        m_mediumSizeAction->setChecked(false);
        m_largeSizeAction->setChecked(false);
    } else if (size >= 32 && size <= 64) {
        m_smallSizeAction->setChecked(false);
        m_mediumSizeAction->setChecked(true);
        m_largeSizeAction->setChecked(false);
    } else if (size > 64) {
        m_smallSizeAction->setChecked(false);
        m_mediumSizeAction->setChecked(false);
        m_largeSizeAction->setChecked(true);
    }
}
