#include "dockpopupwindow.h"

DockPopupWindow::DockPopupWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    setVisible(false);
}

void DockPopupWindow::setContent(QWidget *content)
{
    if (content == nullptr)
        return;

    m_content = content;
    m_content->setParent(this);
    m_content->show();
    m_content->move(0, 0);
}

void DockPopupWindow::show(const QPoint &pos)
{
    m_lastPoint = pos;

    QWidget::move(pos.x(), pos.y());

    setVisible(true);
}

void DockPopupWindow::hide()
{
    setVisible(false);
}
