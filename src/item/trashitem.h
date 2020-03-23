#ifndef TRASHITEM_H
#define TRASHITEM_H

#include "dockitem.h"
#include <QFileSystemWatcher>

class TrashItem : public DockItem
{
    Q_OBJECT

public:
    explicit TrashItem(QWidget *parent = nullptr);

    inline ItemType itemType() const override { return DockItem::Launcher; }

private:
    void refreshIcon();
    void onDirectoryChanged();

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QPixmap m_iconPixmap;
    QFileSystemWatcher *m_filesWatcher;
    int m_count = 0;
};

#endif // TRASHITEM_H
