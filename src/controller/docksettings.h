#ifndef DOCKSETTINGS_H
#define DOCKSETTINGS_H

#include <QSettings>
#include <QObject>
#include <QRect>
#include <QMenu>

#include "dockitemmanager.h"

class DockSettings : public QObject
{
    Q_OBJECT

public:
    enum Position {
        Bottom = 0, Left = 1, Right = 2
    };

    const int PADDING = 5;
    const int MARGIN = 10;

    static DockSettings *instance();
    explicit DockSettings(QObject *parent = nullptr);

    QRect primaryRawRect();
    const QRect windowRect() const;

    void setValue(const QString &key, const QVariant &variant);

    int iconSize() const;

    void showSettingsMenu();

    inline Position position() { return m_position; };

signals:
    void positionChanged();

private:
    QSettings *m_settings;
    Position m_position;
    QMenu *m_settingsMenu;
    QAction *m_leftPosAction;
    QAction *m_rightPosAction;
    QAction *m_bottomPosAction;
};

#endif // DOCKSETTINGS_H
