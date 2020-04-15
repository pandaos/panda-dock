/*
 * Copyright (C) 2018 ~ 2019 PandaOS.
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

#ifndef DESKTOPPROPERTIES_H
#define DESKTOPPROPERTIES_H

#include <QObject>
#include <QVariant>
#include <QMap>

/**
 * @class DesktopProperties
 * @brief Read property files
 * @author Michal Rost
 * @date 26.1.2013
 */

class DesktopProperties
{
public:
    DesktopProperties(const QString &fileName = "", const QString &group = "");
    ~DesktopProperties();

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    bool load(const QString &fileName, const QString &group = "");
    bool save(const QString &fileName, const QString &group = "");
    void set(const QString &key, const QVariant &value);
    bool contains(const QString &key) const;
    QStringList allKeys() const;

protected:
    QMap<QString, QVariant> data;
};

#endif
