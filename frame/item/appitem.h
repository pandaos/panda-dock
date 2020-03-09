#ifndef APPITEM_H
#define APPITEM_H

#include "dockitem.h"
#include "controller/dockitemmanager.h"
#include "controller/appwindowmanager.h"

class AppItem : public DockItem
{
    Q_OBJECT

public:
    explicit AppItem(DockEntry *entry, QWidget *parent = nullptr);

    inline ItemType itemType() const override { return DockItem::App; }
    DockEntry *entry() { return m_entry; };

    void closeWindow();

    void update();

private:
    void initDockAction();
    void dockActionTriggered();
    void refreshIcon();
    void updateWindowIconGeometries();

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    QPixmap m_iconPixmap;
    DockEntry *m_entry;
    QMenu m_contextMenu;
    QAction *m_openAction;
    QAction *m_dockAction;
    QTimer *m_updateIconGeometryTimer;
};

#endif // APPITEM_H
