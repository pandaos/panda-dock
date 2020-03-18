#include "docksettings.h"
#include <QStandardPaths>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QDebug>
#include <math.h>

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
      m_rightPosAction(new QAction(tr("Right"), this)),
      m_bottomPosAction(new QAction(tr("Bottom"), this))
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
    m_rightPosAction->setCheckable(true);
    m_bottomPosAction->setCheckable(true);

    QMenu *positionSubMenu = new QMenu(m_settingsMenu);
    positionSubMenu->addAction(m_leftPosAction);
    positionSubMenu->addAction(m_rightPosAction);
    positionSubMenu->addAction(m_bottomPosAction);

    QAction *positionSubAction = new QAction(tr("Position"), this);
    positionSubAction->setMenu(positionSubMenu);

    m_settingsMenu->addAction(positionSubAction);

    connect(m_leftPosAction, &QAction::triggered, this, [=] {
        m_position = Left;
        setValue("position", Left);
        emit positionChanged();
    });

    connect(m_rightPosAction, &QAction::triggered, this, [=] {
        m_position = Right;
        setValue("position", Right);
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
    QSize size;

    int iconSize = m_settings->value("icon_size").toInt();
    int iconCount = 0;

    for (auto item : DockItemManager::instance()->itemList()) {
        iconCount++;
    }

    // calculate window size.
    switch (m_position) {
    case Bottom:
        size.setHeight(iconSize + PADDING * 4);
        size.setWidth(iconCount * iconSize + (PADDING * 6));
        break;
    case Left: case Right:
        size.setHeight(iconCount * iconSize + (PADDING * 6));
        size.setWidth(iconSize + PADDING * 4);
        break;
    }

    primaryRect.setWidth(std::round(qreal(primaryRect.width()) / scale));
    primaryRect.setHeight(std::round(qreal(primaryRect.height()) / scale));

    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    int margin = 10;
    QPoint p(0, 0);

    switch (m_position) {
    case Bottom:
        p = QPoint(offsetX, primaryRect.height() - size.height() - margin);
        break;
    case Left:
        p = QPoint(margin, offsetY);
        break;
    case Right:
        p = QPoint(primaryRect.width() - size.width() - margin, offsetY);
        break;
    }

    return QRect(primaryRect.topLeft() + p, size);
}

void DockSettings::setValue(const QString &key, const QVariant &variant)
{
    m_settings->setValue(key, variant);
}

int DockSettings::iconSize() const
{
    return m_settings->value("icon_size").toInt();
}

void DockSettings::showSettingsMenu()
{
    m_leftPosAction->setChecked(m_position == Left);
    m_rightPosAction->setChecked(m_position == Right);
    m_bottomPosAction->setChecked(m_position == Bottom);

    m_settingsMenu->exec(QCursor::pos());
}
