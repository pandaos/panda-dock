#include "blurwindow.h"
#include <QPainter>
#include <KWindowEffects>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>

BlurWindow::BlurWindow(QWidget *parent)
    : QWidget(parent),
      m_textLabel(new QLabel)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_textLabel);
    setLayout(layout);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setVisible(false);
}

void BlurWindow::setText(const QString &text)
{
    m_textLabel->setText(text);
}

void BlurWindow::update()
{
    int radius = 16;
    QPainterPath path;
    path.addRoundedRect(this->rect(), radius, radius);
    KWindowEffects::enableBlurBehind(winId(), true, path.toFillPolygon().toPolygon());

    QWidget::update();
}

void BlurWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(QColor(0, 0, 0, 30));
    painter.setBrush(QColor(255, 255, 255, 100));
    int radius = 16;
    painter.drawRoundedRect(rect(), 16, 16);
}
