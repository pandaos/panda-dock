#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QBoxLayout>
#include "appscrollarea.h"
#include "item/dockitem.h"
#include "controller/docksettings.h"
#include "controller/dockitemmanager.h"

class TrashItem;
class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

    void addFixedAreaItem(int index, QWidget *wdg);
    void addAppAreaItem(int index, QWidget *wdg);
    void removeFixedAreaItem(QWidget *wdg);
    void removeAppAreaItem(QWidget *wdg);

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
//    QBoxLayout *m_appAreaLayout;

    QWidget *m_fixedAreaWidget;
//    QWidget *m_appAreaWidget;

    AppScrollArea *m_appArea;
    TrashItem *m_trashItem;

    DockItemManager *m_dockItemmanager;
    DockSettings *m_settings;
};

#endif // MAINPANEL_H
