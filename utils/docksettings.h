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

#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QSettings>
#include <QObject>
#include <QRect>
#include <QMenu>
#include <QFileSystemWatcher>

#include "dockitemmanager.h"

class DockSettings : public QObject
{
    Q_OBJECT

public:
    enum Position {
        Bottom = 0,
        Left = 1,
        Right = 2
    };

    enum Style {
        Classic = 0,
        Fashion
    };

    enum HideMode {
        KeepShowing = 0,
        KeepHidden = 1
    };

    const int PADDING = 10;
    const int MARGIN = 10;

    static DockSettings *instance();
    explicit DockSettings(QObject *parent = nullptr);

    QRect primaryRawRect();
    const QRect windowRect() const;

    void setValue(const QString &key, const QVariant &variant);

    void setIconSize(int size);
    int iconSize() const;

    void setStyle(Style style);
    void setHideMode(HideMode mode);

    void showSettingsMenu();

    inline Position position() { return m_position; };
    inline Style style() { return m_style; };
    inline HideMode hideMode() { return m_hideMode; };

private:
    void initAction();

signals:
    void positionChanged();
    void iconSizeChanged();
    void styleChanged();
    void hideModeChanged();

private:
    QSettings *m_settings;
    Position m_position;
    Style m_style;
    HideMode m_hideMode;
    QMenu *m_settingsMenu;
    QAction *m_leftPosAction;
    QAction *m_bottomPosAction;

    QAction *m_smallSizeAction;
    QAction *m_mediumSizeAction;
    QAction *m_largeSizeAction;

    QAction *m_pcStyleAction;
    QAction *m_padStyleAction;

    QAction *m_keepShowingAction;
    QAction *m_keepHiddenAction;

    QFileSystemWatcher *m_setWatcher;
};

#endif // DOCKSETTINGS_H
