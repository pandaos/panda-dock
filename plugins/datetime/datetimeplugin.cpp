#include "datetimeplugin.h"
#include <QLabel>

DatetimePlugin::DatetimePlugin(QObject *parent)
    : QObject(parent),
      m_datetimeLabel(new QLabel),
      m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setInterval(1000);
    m_refreshTimer->start();

    connect(m_refreshTimer, &QTimer::timeout, this, &DatetimePlugin::updateCurrentTimeString);
}

const QString DatetimePlugin::pluginName() const
{
    return "datetime";
}

const QString DatetimePlugin::pluginDisplayName() const
{
    return tr("Datetime");
}

void DatetimePlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    m_proxyInter->itemAdded(this, QString());
}

QWidget *DatetimePlugin::itemWidget(const QString &itemKey)
{
    return m_datetimeLabel;
}

QWidget *DatetimePlugin::itemTipsWidget(const QString &itemKey)
{
    return nullptr;
}

void DatetimePlugin::updateCurrentTimeString()
{
    const QDateTime currentDateTime = QDateTime::currentDateTime();

    m_datetimeLabel->setText(currentDateTime.toString("HH:mm"));

//    if (m_centralWidget->is24HourFormat())
//        m_datetimeLabel->setText(currentDateTime.date().toString(Qt::SystemLocaleLongDate) + currentDateTime.toString(" HH:mm:ss"));
//    else
//        m_datetimeLabel->setText(currentDateTime.date().toString(Qt::SystemLocaleLongDate) + currentDateTime.toString(" hh:mm:ss A"));
}
