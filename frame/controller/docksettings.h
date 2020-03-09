#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QSettings>
#include <QObject>
#include <QRect>

class DockSettings : public QObject
{
    Q_OBJECT

public:
    static DockSettings *instance();
    explicit DockSettings(QObject *parent = nullptr);

    QRect primaryRawRect();
    const QRect windowRect() const;

    void setValue(const QString &key, const QVariant &variant);
    void setWindowSize(const QSize &size);
    QSize windowSize() const;

private:
    QSettings *m_settings;
    QSize m_windowSize;
};

#endif // DOCKSETTINGS_H
