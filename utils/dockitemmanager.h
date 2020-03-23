#ifndef DOCKITEMMANAGER_H
#define DOCKITEMMANAGER_H

#include "item/dockitem.h"
#include "item/appitem.h"
#include "appwindowmanager.h"

#include <QObject>
#include <QPointer>

class DockItemManager : public QObject
{
    Q_OBJECT

public:
    static DockItemManager *instance(QObject *parent = nullptr);
    explicit DockItemManager(QObject *parent = nullptr);

    const QList<QPointer<DockItem>> itemList() const;

private:
    void appItemAdded(DockEntry *entry);
    void appItemRemoved(DockEntry *entry);
    void appItemActiveChanged(DockEntry *entry);

signals:
    void itemInserted(const int index, DockItem *item) const;
    void itemRemoved(DockItem *item) const;
    void itemUpdated(DockItem *item) const;
    void trayVisableCountChanged(const int &count) const;
    void requestWindowAutoHide(const bool autoHide) const;
    void requestRefershWindowVisible() const;
    void requestScrollToItem(DockItem *item);

private:
    static DockItemManager *INSTANCE;
    AppWindowManager *m_windowManager;
    QList<QPointer<DockItem>> m_itemList;
};

#endif // DOCKITEMMANAGER_H
