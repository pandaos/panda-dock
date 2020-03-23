#include "showdesktopitem.h"
#include "utils/utils.h"
#include <QPainter>
#include <KF5/KWindowSystem/KWindowSystem>

ShowDesktopItem::ShowDesktopItem(QWidget *parent)
    : DockItem(parent)
{
    setAccessibleName("ShowDesktop");
    refreshIcon();
}

void ShowDesktopItem::refreshIcon()
{
    const int iconSize = qMin(width(), height());

    m_iconPixmap = Utils::renderSVG(":/resources/desktop.svg", QSize(iconSize * 0.8, iconSize * 0.8));

    QWidget::update();
}

void ShowDesktopItem::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;

    painter.drawPixmap(iconX, iconY, m_iconPixmap);
}

void ShowDesktopItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void ShowDesktopItem::mouseReleaseEvent(QMouseEvent *e)
{
    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());

    QWidget::mouseReleaseEvent(e);
}
