/*
 * Copyright (C) 2020 PandaOS Team.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "utils/utils.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QTimer>
#include <QEvent>

#include <KF5/KWindowSystem/KWindowEffects>

#include "utils/eventmonitor.h"

const char netWMForceShadow[] = "_PANDA_NET_WM_FORCE_SHADOW";
const char netWMFrameRadius[] = "_PANDA_NET_WM_FRAME_RADIUS";

//static QRegion roundedRect(const QRect& rect, int radius)
//{
//    QRegion region;
//    // middle and borders
//    region += rect.adjusted(radius, 0, -radius, 0);
//    region += rect.adjusted(0, radius, 0, -radius);
//    // top left
//    QRect corner(rect.topLeft(), QSize(radius * 2, radius * 2));
//    region += QRegion(corner, QRegion::Ellipse);
//    // top right
//    corner.moveTopRight(rect.topRight());
//    region += QRegion(corner, QRegion::Ellipse);
//    // bottom left
//    corner.moveBottomLeft(rect.bottomLeft());
//    region += QRegion(corner, QRegion::Ellipse);
//    // bottom right
//    corner.moveBottomRight(rect.bottomRight());
//    region += QRegion(corner, QRegion::Ellipse);
//    return region;
//}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_mainPanel(new MainPanel(this)),
      m_itemManager(DockItemManager::instance()),
      m_settings(DockSettings::instance()),
      //m_fakeWidget(new QWidget(nullptr)),
      m_eventMonitor(new EventMonitor)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_mainPanel);
    setLayout(layout);

//    m_fakeWidget->setFocusPolicy(Qt::NoFocus);
//    m_fakeWidget->setWindowFlags(Qt::FramelessWindowHint);
//    m_fakeWidget->setAttribute(Qt::WA_TranslucentBackground);
//    m_fakeWidget->setVisible(false);

    for (auto item : m_itemManager->itemList())
        m_mainPanel->insertItem(-1, item);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setProperty(netWMForceShadow, true);

    KWindowSystem::setOnDesktop(winId(), NET::OnAllDesktops);
    KWindowSystem::setType(winId(), NET::Dock);

    // initSlideWindow();
    updateSize();

    connect(m_settings, &DockSettings::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_settings, &DockSettings::hideModeChanged, this, &MainWindow::onHideModeChanged);
    connect(m_settings, &DockSettings::styleChanged, this, &MainWindow::updateSize);
    connect(m_settings, &DockSettings::iconSizeChanged, this, &MainWindow::updateSize);
    connect(m_mainPanel, &MainPanel::requestResized, this, &MainWindow::updateSize);
    connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::updateSize, Qt::QueuedConnection);
    connect(m_eventMonitor, &EventMonitor::mouseMove, this, &MainWindow::onMouseMove);
}

MainWindow::~MainWindow()
{

}

void MainWindow::updateSize()
{
    QRect windowRect = m_settings->windowRect();

    // set size.
    QWidget::move(windowRect.left(), windowRect.top());
    QWidget::setFixedSize(windowRect.size());

    delayUpdateBlurRegion();
    updateStrutPartial();
}

void MainWindow::initSlideWindow()
{
    if (m_settings->position() == DockSettings::Left) {
        KWindowEffects::slideWindow(winId(), KWindowEffects::LeftEdge);
    } else if (m_settings->position() == DockSettings::Bottom) {
        KWindowEffects::slideWindow(winId(), KWindowEffects::BottomEdge);
    }
}

void MainWindow::updateStrutPartial()
{
    // blur后设置程序支撑导致模糊无效
//    QRect rect(geometry());
//    rect.setHeight(1);
//    rect.setWidth(1);
//    m_fakeWidget->setGeometry(rect);
//    m_fakeWidget->setVisible(true);

    if (m_settings->hideMode() != DockSettings::KeepShowing) {
        KWindowSystem::setExtendedStrut(winId(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }

    // const auto ratio = devicePixelRatioF();
    const int margin = (m_settings->style() == DockSettings::Classic) ? 1 : 10;

    NETExtendedStrut strut;

    switch (m_settings->position()) {
    case DockSettings::Left:
        strut.left_width = width() + margin;
        strut.left_start = y();
        strut.left_end = y() + height() - 1;
        break;
    case DockSettings::Right:
        strut.right_width = width() + margin;
        strut.right_start = y();
        strut.right_end = y() + height() - 1;
        break;
    case DockSettings::Bottom:
        strut.bottom_width = height() + margin;
        strut.bottom_start = x();
        strut.bottom_end = x() + width();
        break;
    }

    KWindowSystem::setExtendedStrut(winId(),
                                     strut.left_width,
                                     strut.left_start,
                                     strut.left_end,
                                     strut.right_width,
                                     strut.right_start,
                                     strut.right_end,
                                     strut.top_width,
                                     strut.top_start,
                                     strut.top_end,
                                     strut.bottom_width,
                                     strut.bottom_start,
                                     strut.bottom_end);

//    KWindowSystem::setState(m_fakeWidget->winId(), NET::SkipTaskbar);
//    KWindowSystem::setState(m_fakeWidget->winId(), NET::SkipSwitcher);

    QWidget::update();
}

void MainWindow::onPositionChanged()
{
    updateSize();

    // Need to update the geometry of the appitem.
}

void MainWindow::onHideModeChanged()
{
    if (m_settings->hideMode() == DockSettings::KeepShowing)
        setVisible(true);
    else {
        m_eventMonitor->start();
        setVisible(false);
    }

    updateStrutPartial();
}

void MainWindow::onMouseMove(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    // if (!onScreenEdge(QPoint(x, y))) {
    //     return;
    // } else {
    // }

    setVisible(true);
}

void MainWindow::updateBlurRegion()
{
    QPainterPath path;

    if (m_settings->style() == DockSettings::Classic) {
        path = getCornerPath();
        setProperty(netWMFrameRadius, m_frameRadius);
    } else {
        const qreal radius = std::min(rect().width(), rect().height()) * m_settings->radiusRatio();
        path.addRoundedRect(this->rect(), radius, radius);
        setProperty(netWMFrameRadius, radius);
    }

    QPolygon polygon = path.toFillPolygon().toPolygon();
    KWindowEffects::enableBlurBehind(winId(), true, polygon);

    // update shadow
    qApp->postEvent(this, new QEvent(QEvent::WinIdChange));
}

void MainWindow::delayUpdateBlurRegion()
{
    QTimer::singleShot(0, this, &MainWindow::updateBlurRegion);
}

QPainterPath MainWindow::getCornerPath()
{
    const QRect r = this->rect();
    QPainterPath path;

    path.setFillRule(Qt::WindingFill);

    if (m_settings->position() == DockSettings::Left) {
        path.addRoundedRect(0, 0, r.width(), r.height(), m_frameRadius, m_frameRadius);
        path.addRect(QRect(0, 0, r.width() / 2, r.height()));
    } else {
        path.addRoundedRect(0, 0, r.width(), r.height(), m_frameRadius, m_frameRadius);
        path.addRect(QRect(0, r.height() / 2, r.width(), r.height()));
    }

    return path.simplified();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(255, 255, 255, 150));
    painter.setBrush(QColor(255, 255, 255, 100));

    const QPalette &palette = this->palette();
    QColor backgroundColor = palette.color(QPalette::Window);
    const bool light = backgroundColor.lightness() > 128;
    // QColor outlineColor = light ? QColor(0, 0, 0, 60) : QColor(255, 255, 255, 140);
    backgroundColor.setAlpha(100);

    // painter.setPen(outlineColor);
    painter.setBrush(backgroundColor);

    if (m_settings->style() == DockSettings::Classic) {
        painter.drawPath(getCornerPath());
    } else {
        const qreal radius = std::min(rect().width(), rect().height()) * m_settings->radiusRatio();
        // painter.drawRoundedRect(rect(), radius, radius);

        QPainterPath path;
        path.addRoundedRect(rect(), radius, radius);
        painter.setClipPath(path);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillPath(path, backgroundColor);
        // painter.strokePath(path, outlineColor);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    e->ignore();

    if (e->button() == Qt::RightButton) {
        m_settings->showSettingsMenu();
    }
}
