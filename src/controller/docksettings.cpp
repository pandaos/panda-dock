#include "docksettings.h"
#include <QStandardPaths>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QDebug>
#include <math.h>

#define TOPBARHEIGHT 40

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
    int iconSize = m_settings->value("icon_size").toInt();
    int iconCount = DockItemManager::instance()->itemList().count();
    QSize size;

    if (m_position != Bottom) {
        primaryRect.setHeight(primaryRect.height() - TOPBARHEIGHT);
        primaryRect.setY(TOPBARHEIGHT);
    }

    const int maxWidth = primaryRect.width() - MARGIN * 4;
    const int maxHeight = primaryRect.height() - MARGIN * 4;
    const int calcWidth = iconCount * iconSize + PADDING * 4;
    const int calcHeight = iconSize + PADDING * 2;

    // calculate window size.
    switch (m_position) {
    case Bottom:
        size.setHeight(calcHeight);
        size.setWidth(qMin(maxWidth, calcWidth));
        break;
    case Left: case Right:
        size.setHeight(qMin(maxHeight, calcWidth));
        size.setWidth(calcHeight);
        break;
    }

    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    int margin = 14;
    QPoint p(0, 0);

    switch (m_position) {
    case Bottom:
        p = QPoint(offsetX, primaryRect.height() - size.height() - margin);
        break;
    case Left:
        p = QPoint(margin, offsetY + TOPBARHEIGHT / 2);
        break;
//    case Right:
//        p = QPoint(primaryRect.width() - size.width() - margin, offsetY);
//        break;
    }

    return QRect(primaryRect.topLeft() + p, size);
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
