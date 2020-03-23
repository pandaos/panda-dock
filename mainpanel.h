#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QBoxLayout>
#include "appscrollarea.h"
#include "item/dockitem.h"
#include "utils/docksettings.h"
#include "utils/dockitemmanager.h"

class TrashItem;
class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

    void addFixedAreaItem(int index, QWidget *wdg);
    void removeFixedAreaItem(QWidget *wdg);

signals:
    void requestResized();

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

private:
    void init();
    void updateLayout();
    void resizeDockIcon();
    void onPositionChanged();

    void scrollToItem(DockItem *item);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QBoxLayout *m_mainLayout;
    QBoxLayout *m_fixedAreaLayout;

    QWidget *m_fixedAreaWidget;
    AppScrollArea *m_appArea;
    TrashItem *m_trashItem;

    DockItemManager *m_dockItemmanager;
    DockSettings *m_settings;
};

#endif // MAINPANEL_H
