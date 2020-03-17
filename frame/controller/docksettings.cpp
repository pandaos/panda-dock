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

    if (!m_settings->contains("window_bottom_size")) {
        m_settings->setValue("window_bottom_size", QSize(50, 50));
    }

    if (!m_settings->contains("window_left_size")) {
        m_settings->setValue("window_left_size", QSize(50, 50));
    }

    if (!m_settings->contains("position")) {
        m_settings->setValue("position", Bottom);
    }

    m_bottomSize = m_settings->value("window_bottom_size").toSize();
    m_leftSize = m_settings->value("window_left_size").toSize();
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

    // calculate window size.
    switch (m_position) {
    case Bottom:
        size.setHeight(m_bottomSize.height());
        size.setWidth(primaryRect.width());
        break;
    case Left: case Right:
        size.setHeight(primaryRect.height());
        size.setWidth(m_leftSize.width());
        primaryRect.setY(40);
        primaryRect.setHeight(primaryRect.height() - 41);
        break;
    }

    primaryRect.setWidth(std::round(qreal(primaryRect.width()) / scale));
    primaryRect.setHeight(std::round(qreal(primaryRect.height()) / scale));

    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    int margin = 0;
    QPoint p(0, 0);

    switch (m_position) {
    case Bottom:
        p = QPoint(offsetX, primaryRect.height() - size.height());
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

void DockSettings::setWindowBottomSize(const QSize &size)
{
    m_settings->setValue("window_bottom_size", size);
    m_bottomSize = size;
}

void DockSettings::setWindowLeftSize(const QSize &size)
{
    m_settings->setValue("window_left_size", size);
    m_leftSize = size;
}

QSize DockSettings::windowSize() const
{
    if (m_position == Bottom) {
        return m_settings->value("window_bottom_size").toSize();
    } else {
        return m_settings->value("window_left_size").toSize();
    }
}

void DockSettings::showSettingsMenu()
{
    m_leftPosAction->setChecked(m_position == Left);
    m_rightPosAction->setChecked(m_position == Right);
    m_bottomPosAction->setChecked(m_position == Bottom);

    m_settingsMenu->exec(QCursor::pos());
}
