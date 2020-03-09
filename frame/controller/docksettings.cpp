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
                               .arg(qApp->applicationName()), QSettings::IniFormat))
{
    qDebug() << QString("%1/%2/%3/config.conf")
                .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                .arg(qApp->organizationName())
                .arg(qApp->applicationName());

    if (!m_settings->contains("window_size")) {
        m_settings->setValue("window_size", QSize(50, 50));
    }

    m_windowSize = m_settings->value("window_size").toSize();
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
    size.setHeight(m_windowSize.height());
    size.setWidth(primaryRect.width());

    primaryRect.setWidth(std::round(qreal(primaryRect.width()) / scale));
    primaryRect.setHeight(std::round(qreal(primaryRect.height()) / scale));

    const int offsetX = (primaryRect.width() - size.width()) / 2;
    const int offsetY = (primaryRect.height() - size.height()) / 2;
    QPoint p(0, 0);

    // bottom
    p = QPoint(offsetX, primaryRect.height() - size.height());
    QRect windowRect(primaryRect.topLeft() + p, size);

    return windowRect;
}

void DockSettings::setValue(const QString &key, const QVariant &variant)
{
    m_settings->setValue(key, variant);
}

void DockSettings::setWindowSize(const QSize &size)
{
    m_settings->setValue("window_size", size);
    m_windowSize = size;
}

QSize DockSettings::windowSize() const
{
    return m_settings->value("window_size").toSize();
}
