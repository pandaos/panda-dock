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

#include "appwindowmanager.h"
#include <QRegularExpression>
#include <QStandardPaths>
#include <QApplication>
#include <QX11Info>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QFileInfo>

static const NET::Properties windowInfoFlags = NET::WMState | NET::XAWMState | NET::WMDesktop |
             NET::WMVisibleName | NET::WMGeometry | NET::WMWindowType;
static const NET::Properties2 windowInfoFlags2 = NET::WM2WindowClass | NET::WM2AllowedActions | NET::WM2DesktopFileName;

AppWindowManager *AppWindowManager::instance()
{
    static AppWindowManager INSTANCE;

    return &INSTANCE;
}

AppWindowManager::AppWindowManager(QObject *parent)
    : QObject(parent)
{
    m_currentDesktop = KWindowSystem::self()->currentDesktop();
    m_settings = new QSettings(QString("%1/%2/dock_list.conf")
                               .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                               .arg(qApp->applicationName()), QSettings::IniFormat);
    m_setWatcher = new QFileSystemWatcher(this);

    connect(m_setWatcher, &QFileSystemWatcher::fileChanged, this, &AppWindowManager::refreshDockedList);

    m_setWatcher->addPath(QString("%1/%2/dock_list.conf")
                          .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                          .arg(qApp->applicationName()));

    refreshWindowList();

    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &AppWindowManager::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &AppWindowManager::onWindowRemoved);
//    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AppWindowManager::onActiveWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::currentDesktopChanged, this, [&](int desktop) {
        m_currentDesktop = desktop;
    });

    //    connect(KWindowSystem::self(), &KWindowSystem::windowChanged, this, &AppWindowManager::onWindowChanged);
}

DockEntry *AppWindowManager::find(quint64 id)
{
    for (DockEntry *entry : m_dockList) {
        for (quint64 winId : entry->WIdList) {
            if (winId == id) {
                return entry;
            }
        }
    }

    return nullptr;
}

DockEntry *AppWindowManager::findByClassName(const QString &className)
{
    for (DockEntry *entry : m_dockList) {
        if (entry->className == className)
            return entry;
    }

    return nullptr;
}

int AppWindowManager::getPid(quint64 winId)
{
    return NETWinInfo(QX11Info::connection(), winId, QX11Info::appRootWindow(), NET::WMPid).pid();
}

bool AppWindowManager::classNameContains(const QString &className)
{
    for (DockEntry *entry : m_dockList) {
        if (entry->className == className)
            return true;
    }

    return false;
}

bool AppWindowManager::isAcceptWindow(quint64 id) const
{
    QFlags<NET::WindowTypeMask> ignoreList;
    ignoreList |= NET::DesktopMask;
    ignoreList |= NET::DockMask;
    ignoreList |= NET::SplashMask;
    ignoreList |= NET::ToolbarMask;
    ignoreList |= NET::MenuMask;
    ignoreList |= NET::PopupMenuMask;
    ignoreList |= NET::NotificationMask;

    KWindowInfo info(id, NET::WMWindowType | NET::WMState, NET::WM2TransientFor | NET::WM2WindowClass);

    // 过滤自身
    if (info.windowClassClass().toLower() == "panda-dock")
        return false;

    if (!info.valid())
        return false;

    if (NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
        return false;

    if (info.state() & NET::SkipTaskbar)
        return false;

    // WM_TRANSIENT_FOR hint not set - normal window
    WId transFor = info.transientFor();
    if (transFor == 0 || transFor == id || transFor == (WId) QX11Info::appRootWindow())
        return true;

    info = KWindowInfo(transFor, NET::WMWindowType);

    QFlags<NET::WindowTypeMask> normalFlag;
    normalFlag |= NET::NormalMask;
    normalFlag |= NET::DialogMask;
    normalFlag |= NET::UtilityMask;

    return !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), normalFlag);
}

void AppWindowManager::triggerWindow(quint64 id)
{
    KWindowInfo info(id, NET::WMDesktop | NET::WMState | NET::XAWMState);

    if (info.isMinimized()) {
        bool onCurrent = info.isOnDesktop(m_currentDesktop);

        KWindowSystem::unminimizeWindow(id);

        if (onCurrent) {
            KWindowSystem::forceActiveWindow(id);
        }
    } else {
        KWindowSystem::minimizeWindow(id);
    }
}

void AppWindowManager::minimizeWindow(quint64 id)
{
    KWindowSystem::minimizeWindow(id);
}

void AppWindowManager::raiseWindow(quint64 id)
{
    KWindowInfo info(id, NET::WMDesktop | NET::WMState | NET::XAWMState);
    int desktop = info.desktop();

    if (KWindowSystem::currentDesktop() != desktop)
        KWindowSystem::setCurrentDesktop(desktop);

    KWindowSystem::activateWindow(id);
}

void AppWindowManager::closeWindow(quint64 id)
{
    // FIXME: Why there is no such thing in KWindowSystem??
    NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(id);
}

void AppWindowManager::openApp(const QString &appName)
{
    QProcess::startDetached(appName);
}

void AppWindowManager::clicked(DockEntry *entry)
{
    if (entry->WIdList.isEmpty()) {
        qDebug() << "clicked: " << entry->exec;
        openApp(entry->exec);
    } else if (entry->WIdList.count() > 1) {
        qDebug() << "current: " << entry->current << ", list count: " << entry->WIdList.count();

        entry->current++;

        if (entry->current == entry->WIdList.count()) {
            entry->current = 0;
        }
        KWindowSystem::forceActiveWindow(entry->WIdList.at(entry->current));

    } else if (KWindowSystem::activeWindow() == entry->WIdList.first()) {
        KWindowSystem::minimizeWindow(entry->WIdList.first());
    } else {
        KWindowSystem::forceActiveWindow(entry->WIdList.first());
    }
}

void AppWindowManager::undock(DockEntry *entry)
{
    if (entry->WIdList.isEmpty()) {
        m_dockList.removeOne(entry);
        emit entryRemoved(entry);
    }
}

void AppWindowManager::save()
{
    QStringList docked;
    for (DockEntry *entry : m_dockList) {
        if (entry->isDocked) {
            docked.append(entry->className);
        }
    }

    qDebug() << "saved: " << docked;

    m_setWatcher->blockSignals(true);
    m_settings->setValue("appname", QVariant::fromValue(docked));
    m_setWatcher->blockSignals(false);
}

void AppWindowManager::initEntry(DockEntry *entry)
{
    // 通过 window class class 找到系统 desktop 文件, 读取真正的图标名词exec等字段
    // 并且存储方便下次直接读取
    const QString &key = entry->className;
    QSettings set(QString("%1/%2/appinfo.conf")
                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                  .arg(qApp->applicationName()), QSettings::IniFormat);
    set.setIniCodec("UTF8");
    set.beginReadArray(key);

    if (set.contains("Desktop")) {
        entry->visibleName = set.value("Name").toString();
        entry->iconName = set.value("Icon").toString();
        entry->exec = set.value("Exec").toString();
        return;
    }

    QDir dir("/usr/share/applications");
    for (const QFileInfo &info : dir.entryInfoList(QDir::Files)) {
        // 必须是 desktop 文件
        if (info.suffix() == "desktop") {
            QSettings settings(info.filePath(), QSettings::IniFormat);
            settings.beginGroup("Desktop Entry");
            settings.setIniCodec("UTF8");

            if (settings.value("NoDisplay").toBool()) {
                continue;
            }

            const QString &iconValue = settings.value("Icon").toString();
            QString execValue = settings.value("Exec").toString();
            QString nameValue = settings.value("Name").toString();

            execValue.remove(QRegularExpression("%."));
            execValue.remove(QRegularExpression("^\""));
            execValue.remove(QRegularExpression(" *$"));

            bool founded = false;

            if (info.baseName() == key) {
                founded = true;
            }

            // Try matching both appId and xWindowsWMClassName against StartupWMClass.
            // We do this before evaluating the mapping rules further, because StartupWMClass
            // is essentially a mapping rule, and we expect it to be set deliberately and
            // sensibly to instruct us what to do. Also, mapping rules
            //
            // StartupWMClass=STRING
            //
            //   If true, it is KNOWN that the application will map at least one
            //   window with the given string as its WM class or WM name hint.
            //
            // Source: https://specifications.freedesktop.org/startup-notification-spec/startup-notification-0.1.txt
            if (!founded && settings.value("StartupWMClass").toString().startsWith(key, Qt::CaseInsensitive)) {
                founded = true;
            }

            if (!founded && iconValue.startsWith(key, Qt::CaseInsensitive)) {
                founded = true;
            }

            // Try matching mapped name against 'Name'.
            if (!founded && nameValue.startsWith(key, Qt::CaseInsensitive)) {
                founded = true;
            }

            if (!founded && execValue.startsWith(key, Qt::CaseInsensitive)) {
                founded = true;
            }

            if (!founded && info.baseName().startsWith(key, Qt::CaseInsensitive)) {
                founded = true;
            }

            if (founded) {
                if (!settings.value(QString("Name[%1]").arg(QLocale::system().name())).toString().isEmpty()) {
                    nameValue = settings.value(QString("Name[%1]").arg(QLocale::system().name())).toString();
                }
                entry->visibleName = nameValue;
                entry->iconName = iconValue;
                entry->exec = execValue;

//                set.beginWriteArray(key);
                set.beginGroup(key);
                set.setValue("Icon", iconValue);
                set.setValue("Exec", execValue);
                set.setValue("Desktop", info.filePath());
                // TODO: 多语言
                set.setValue("Name", nameValue);
                set.endGroup();
                set.sync();
            }
        }
    }
}

void AppWindowManager::refreshWindowList()
{
    QSettings set;

    m_settings->sync();
    QStringList dockedList = m_settings->value("appname").value<QStringList>();

    for (QString &var : dockedList) {
        qDebug() << var << " init";
        DockEntry *entry = new DockEntry;
        entry->className = var;
        entry->isDocked = true;
        entry->exec = var;
        entry->iconName = getIcon(var);
        initEntry(entry);
        m_dockList.append(entry);

        emit entryAdded(entry);
    }

    for (auto wid : KWindowSystem::self()->windows()) {
        onWindowAdded(wid);
    }
}

void AppWindowManager::refreshDockedList()
{
    QSettings set(QString("%1/%2/dock_list.conf")
                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                  .arg(qApp->applicationName()), QSettings::IniFormat);
    QStringList dockedList = set.value("appname").value<QStringList>();

    for (QString val : dockedList) {
        if (!classNameContains(val)) {
            DockEntry *e = new DockEntry;
            e->className = val;
            e->isDocked = true;
            e->exec = val;
            e->iconName = getIcon(val);
            initEntry(e);
            m_dockList.append(e);
            emit entryAdded(e);
        }
    }

//    for (DockEntry *entry : m_dockList) {
//        if (!dockedList.contains(entry->className)) {
//            if (entry->WIdList.isEmpty()) {
//                m_dockList.removeOne(entry);
//                emit entryRemoved(entry);
//                entry->isDocked = false;
//            } else {
//            }
//        }
//    }

    m_setWatcher->addPath(QString("%1/%2/dock_list.conf")
                          .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                          .arg(qApp->applicationName()));
}

QString AppWindowManager::getExec(quint64 id)
{
    NETWinInfo info(QX11Info::connection(), id, QX11Info::appRootWindow(), NET::WMPid, NET::WM2DesktopFileName);
    QFile file(QString("/proc/") + QString::number(info.pid()) + QString("/cmdline"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    return whichCmd(QString::fromUtf8(file.readAll()));
}

QString AppWindowManager::whichCmd(const QString &cmd)
{
    // emulated the which command for programs started in cmd
    QString bin;

    for(int i = 0; i < cmd.size(); i++) {
        const QChar ch = cmd[i];
        if(ch == '\\')
            i++;
        else if (ch == ' ')
            break;
        else
            bin += ch;
    }

    if(bin.startsWith("/"))
        return bin;

    const QStringList pathEnv = QString(qgetenv("PATH").constData()).split(":");

    for (const QString &path : pathEnv) {
        if(QFile::exists(path + "/" + bin)) {
            return path + "/" + bin;
        }
    }

    return bin;
}

QString AppWindowManager::getDesktop(const QString &keyword)
{
    QDir dir("/usr/share/applications");
    QFileInfoList list = dir.entryInfoList(QStringList() << "*.desktop", QDir::Files);

    for (const QFileInfo &info : list) {
        if (info.fileName().contains(keyword, Qt::CaseInsensitive)) {
            return info.filePath();
        }
    }

    return QString();
}

QString AppWindowManager::getIcon(const QString &exec)
{
    const QString &desktopPath = getDesktop(exec);

    if (desktopPath.isEmpty())
        return QString();

    QSettings settings(desktopPath, QSettings::IniFormat);
    QString iconName;
    settings.beginGroup("Desktop Entry");
    iconName = settings.value("Icon").toString();
    settings.endGroup();

    return iconName;
}

void AppWindowManager::onWindowAdded(quint64 id)
{
    if (!isAcceptWindow(id))
        return;

    KWindowInfo info(id, windowInfoFlags, windowInfoFlags2);

    const QString &className = QString::fromUtf8(info.windowClassClass().toLower());
    if (classNameContains(className)) {
        DockEntry *entry = nullptr;
        for (DockEntry *e : m_dockList) {
            if (e->className == className) {
                entry = e;
            }
        }

        if (entry) {
            entry->WIdList.append(id);
            entry->current = 0;
            initEntry(entry);
        }

        qDebug() << "added: " << id << entry->className << entry->isActive << entry->exec;

    } else {
        DockEntry *entry = new DockEntry;
        entry->WIdList.append(id);
        entry->className = QString::fromUtf8(info.windowClassClass().toLower());
        entry->isActive = KWindowSystem::activeWindow() == id;
        entry->visibleName = info.visibleName();
        entry->exec = entry->className;
        entry->iconName = getIcon(entry->className);
        initEntry(entry);

        m_dockList.append(entry);

        qDebug() << "added: " << id << entry->className << entry->isActive << entry->exec << " new";

    //    KWindowInfo info(mWindow, NET::WMVisibleName | NET::WMName);
    //    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
    //    setText(title.replace(QStringLiteral("&"), QStringLiteral("&&")));

        emit entryAdded(entry);
    }
}

void AppWindowManager::onWindowRemoved(quint64 id)
{
    DockEntry *e = find(id);

    if (e) {
        e->WIdList.removeOne(id);
        e->current = 0;
        if (e->WIdList.isEmpty()) {
            if (!e->isDocked) {
                m_dockList.removeOne(e);
                emit entryRemoved(e);
            } else {
                emit activeChanged(e);
            }
        }
    }
}

void AppWindowManager::onActiveWindowChanged(quint64 id)
{
    KWindowInfo info(id, windowInfoFlags, windowInfoFlags2);

    if (!info.valid(true))
        return;

    for (auto entry : m_dockList) {
        for (quint64 wid : entry->WIdList) {
            if (wid == id) {
                entry->isActive = true;
                break;
            } else {
                entry->isActive = false;
            }
        }

        emit activeChanged(entry);
    }
}

void AppWindowManager::onWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2)
{

}
