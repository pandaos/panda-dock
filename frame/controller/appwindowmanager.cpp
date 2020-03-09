#include "appwindowmanager.h"
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QApplication>
#include <QX11Info>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QFile>

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
    m_settings = new QSettings(QString("%1/%2/%3/dock_list.conf")
                               .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                               .arg(qApp->organizationName())
                               .arg(qApp->applicationName()), QSettings::IniFormat);

    refreshWindowList();

    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &AppWindowManager::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &AppWindowManager::onWindowRemoved);
    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AppWindowManager::onActiveWindowChanged);
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

bool AppWindowManager::iconNameContains(const QString &iconName)
{
    for (DockEntry *entry : m_dockList) {
        if (entry->icon == iconName)
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

    KWindowInfo info(id, NET::WMWindowType | NET::WMState, NET::WM2TransientFor);

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
    QStringList exec;
    for (DockEntry *entry : m_dockList) {
        if (entry->isDocked) {
            docked.append(entry->icon);
            exec.append(entry->exec);
        }
    }

    qDebug() << "saved: " << docked;

    m_settings->setValue("appname", QVariant::fromValue(docked));
    m_settings->setValue("appexec", QVariant::fromValue(exec));
}

void AppWindowManager::refreshWindowList()
{
    QStringList dockedList = m_settings->value("appname").value<QStringList>();
    QStringList execList = m_settings->value("appexec").value<QStringList>();

    for (QString &var : dockedList) {
        qDebug() << var << " init";
        DockEntry *entry = new DockEntry;
        entry->icon = var;
        entry->isDocked = true;
        entry->exec = execList.at(dockedList.indexOf(var));
        m_dockList.append(entry);

        emit entryAdded(entry);
    }

    for (auto wid : KWindowSystem::self()->windows()) {
        onWindowAdded(wid);
    }
}

QString AppWindowManager::getExec(quint64 id)
{
    NETWinInfo info(QX11Info::connection(), id, QX11Info::appRootWindow(), NET::WMPid, NET::WM2DesktopFileName);
    const char *desktopFile = info.desktopFileName();
    if(desktopFile != nullptr) {
        // some Qt applications provide _KDE_NET_WM_DESKTOP_FILE atom

        return "";
    }

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
    foreach (const QString &path, pathEnv) {
        if(QFile::exists(path + "/" + bin)) {
            return path + "/" + bin;
        }
    }

    return bin;
}

void AppWindowManager::onWindowAdded(quint64 id)
{
    if (!isAcceptWindow(id))
        return;

    KWindowInfo info(id, windowInfoFlags, windowInfoFlags2);

    const QString &iconName = QString::fromUtf8(info.windowClassClass().toLower());
    if (iconNameContains(iconName)) {
        DockEntry *entry = nullptr;
        for (DockEntry *e : m_dockList) {
            if (e->icon == iconName) {
                entry = e;
            }
        }

        if (entry) {
            if (entry->exec.isEmpty()) {
                entry->exec = getExec(id);
            }
            entry->WIdList.append(id);
            entry->current = 0;
        }

        qDebug() << "added: " << id << entry->icon << entry->isActive << entry->exec;

    } else {
        DockEntry *entry = new DockEntry;
        entry->WIdList.append(id);
        entry->icon = QString::fromUtf8(info.windowClassClass().toLower());
        entry->id = QCryptographicHash::hash(entry->icon.toUtf8(), QCryptographicHash::Md5).toHex();
        entry->isActive = KWindowSystem::activeWindow() == id;
        entry->name = info.visibleName();
        entry->exec = getExec(id);

        m_dockList.append(entry);

        qDebug() << "added: " << id << entry->icon << entry->isActive << entry->exec;

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
