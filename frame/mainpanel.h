#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QBoxLayout>
#include "item/dockitem.h"
#include "controller/dockitemmanager.h"

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

    void addFixedAreaItem(int index, QWidget *wdg);
    void addAppAreaItem(int index, QWidget *wdg);
    void addTrayAreaItem(int index, QWidget *wdg);
    void addPluginAreaItem(int index, QWidget *wdg);
    void removeFixedAreaItem(QWidget *wdg);
    void removeAppAreaItem(QWidget *wdg);
    void removeTrayAreaItem(QWidget *wdg);
    void removePluginAreaItem(QWidget *wdg);

public slots:
    void insertItem(const int index, DockItem *item);
    void removeItem(DockItem *item);
    void itemUpdated(DockItem *item);

private:
    void init();
    void updateLayout();
    void resizeDockIcon();
    void calcuDockIconSize(int w, int h);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QBoxLayout *m_mainLayout;
    QBoxLayout *m_fixedAreaLayout;
    QBoxLayout *m_appAreaLayout;
    QBoxLayout *m_trayAreaLayout;
    QBoxLayout *m_pluginAreaLayout;

    QWidget *m_fixedAreaWidget;
    QWidget *m_appAreaWidget;
    QWidget *m_trayAreaWidget;
    QWidget *m_pluginAreaWidget;

    DockItemManager *m_dockItemmanager;
};

#endif // MAINPANEL_H
