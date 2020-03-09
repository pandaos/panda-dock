#ifndef DOCKPOPUPWINDOW_H
#define DOCKPOPUPWINDOW_H

#include <QWidget>

class DockPopupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DockPopupWindow(QWidget *parent = nullptr);

    void setContent(QWidget *content);
    void show(const QPoint &pos);
    void hide();

private:
    QPoint m_lastPoint;
    QWidget *m_content;
};

#endif // DOCKPOPUPWINDOW_H
