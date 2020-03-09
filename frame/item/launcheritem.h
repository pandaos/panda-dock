#ifndef LAUNCHERITEM_H
#define LAUNCHERITEM_H

#include "dockitem.h"

class LauncherItem : public DockItem
{
    Q_OBJECT

public:
    explicit LauncherItem(QWidget *parent = nullptr);

    inline ItemType itemType() const override { return DockItem::Launcher; }

private:
    void refreshIcon();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QPixmap m_iconPixmap;
};

#endif // LAUNCHERITEM_H
