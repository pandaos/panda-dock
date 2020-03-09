#ifndef DOCKITEM_H
#define DOCKITEM_H

#include <QWidget>
#include <QPointer>
#include <QMenu>
#include "components/hoverhighlighteffect.h"
#include "utils/dockpopupwindow.h"

class DockItem : public QWidget
{
    Q_OBJECT

public:
    enum ItemType {
        Launcher,
        App,
        Plugins,
        FixedPlugin,
        Placeholder,
        TrayPlugin,
    };

    explicit DockItem(QWidget *parent = nullptr);

    inline virtual ItemType itemType() const { Q_UNREACHABLE(); return App; }

    QSize sizeHint() const override;

    virtual const QString contextMenu() const;
    const QRect perfectIconRect() const;

    virtual QWidget *popupTips();

    virtual void showHoverTips();

private:
    void showContextMenu();
    const QPoint popupMarkPoint() const;
    const QPoint topleftPoint() const;

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QPointer<HoverHighlightEffect> m_hoverEffect;
    QPointer<DockPopupWindow> m_popupWindow;
    QTimer *m_popupTipsDelayTimer;
    QMenu m_contextMenu;
    bool m_hover;
};

#endif // DOCKITEM_H
