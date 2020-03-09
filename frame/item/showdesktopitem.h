#ifndef SHOWDESKTOPITEM_H
#define SHOWDESKTOPITEM_H

#include "dockitem.h"

class ShowDesktopItem : public DockItem
{
    Q_OBJECT

public:
    explicit ShowDesktopItem(QWidget *parent = nullptr);

    inline virtual ItemType itemType() const { return Launcher; }

private:
    void refreshIcon();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QPixmap m_iconPixmap;
};

#endif // SHOWDESKTOPITEM_H
