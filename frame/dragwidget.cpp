#include "dragwidget.h"
#include <QMouseEvent>
#include <QApplication>

DragWidget::DragWidget(QWidget *parent)
    : QWidget(parent)
{
    m_dragStatus = false;
}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizePoint = event->globalPos();
        m_dragStatus = true;
        this->grabMouse();
    }
}

void DragWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragStatus) {
        QPoint offset = QPoint(QCursor::pos() - m_resizePoint);
        emit dragPointOffset(offset);
    }
}

void DragWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_dragStatus)
        return;

    m_dragStatus =  false;
    releaseMouse();
    emit dragFinished();
}

void DragWidget::enterEvent(QEvent *)
{
    if (QApplication::overrideCursor() && QApplication::overrideCursor()->shape() != cursor()) {
        QApplication::setOverrideCursor(cursor());
    }
}

void DragWidget::leaveEvent(QEvent *)
{
    QApplication::restoreOverrideCursor();
}
