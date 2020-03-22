#include "appscrollarea.h"
#include <QWheelEvent>
#include <QScrollBar>
#include <QScroller>
#include <QDebug>

AppScrollArea::AppScrollArea(QWidget *parent)
    : QScrollArea(parent),
      m_mainLayout(new QBoxLayout(QBoxLayout::LeftToRight)),
      m_mainWidget(new QWidget),
      m_range(10)
{
    m_mainWidget->setLayout(m_mainLayout);

    setAttribute(Qt::WA_TranslucentBackground);
    setFrameShape(QFrame::NoFrame);
    setWidget(m_mainWidget);
    setWidgetResizable(true);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
//    m_mainWidget->setAutoFillBackground(false);

    setStyleSheet("background: transparent;");

    setContentsMargins(0, 0, 0, 0);

    QScroller::grabGesture(m_mainWidget);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void AppScrollArea::addItem(AppItem *item)
{
    m_mainLayout->addWidget(item);
    m_mainWidget->adjustSize();
}

void AppScrollArea::removeItem(AppItem *item)
{
    m_mainLayout->removeWidget(item);
}

void AppScrollArea::scrollToItem(AppItem *item)
{
    return;
    blockSignals(true);
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        horizontalScrollBar()->setValue(item->geometry().x());
    } else {
        verticalScrollBar()->setValue(item->geometry().y());
    }
    blockSignals(false);
}

void AppScrollArea::setRange(int value)
{
    m_range = value;
}

void AppScrollArea::wheelEvent(QWheelEvent *e)
{
    if (m_mainLayout->direction() == QBoxLayout::LeftToRight) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range));
    } else {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (e->angleDelta().y() / 120.0 * m_range));
    }
    return;

    QScrollArea::wheelEvent(e);
}
