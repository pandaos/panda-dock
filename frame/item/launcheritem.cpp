#include "launcheritem.h"
#include "utils/utils.h"
#include <QPainter>
#include <QProcess>

LauncherItem::LauncherItem(QWidget *parent)
    : DockItem(parent)
{
    setAccessibleName("Launcher");
    refreshIcon();
}

void LauncherItem::refreshIcon()
{
    const int iconSize = qMin(width(), height());

    m_iconPixmap = Utils::renderSVG(":/resources/launcher.svg", QSize(iconSize * 0.8, iconSize * 0.8));

    QWidget::update();
}

void LauncherItem::paintEvent(QPaintEvent *e)
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

void LauncherItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void LauncherItem::mouseReleaseEvent(QMouseEvent *e)
{
    QProcess::startDetached("pandaos-launcher");
    QWidget::mouseReleaseEvent(e);
}
