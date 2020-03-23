#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "mainpanel.h"
#include "utils/dockitemmanager.h"
#include "utils/docksettings.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initWindowSize();

private:
    void setStrutPartial();
    void onPositionChanged();

    void resizeEvent(QResizeEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    MainPanel *m_mainPanel;
    DockItemManager *m_itemManager;
    DockSettings *m_settings;
//    XcbMisc *m_xcbMisc;
    QWidget *m_fakeWidget;
    QSize m_size;
};

#endif // MAINWINDOW_H
