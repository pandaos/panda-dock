#include "mainwindow.h"
#include "utils/utils.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QScreen>
#include <QPainter>

#include <KF5/KWindowSystem/KWindowEffects>

#define MAINWINDOW_MAX_SIZE       100
#define MAINWINDOW_MIN_SIZE       (45)
#define MAINWINDOW_PADDING (1)
#define DRAG_AREA_SIZE (5)

#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <fixx11h.h>

const QPoint rawXPosition(const QPoint &scaledPos)
{
    QScreen const *screen = Utils::screenAtByScaled(scaledPos);

    return screen ? screen->geometry().topLeft() +
           (scaledPos - screen->geometry().topLeft()) *
           screen->devicePixelRatio()
           : scaledPos;
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_mainPanel(new MainPanel),
      m_dragWidget(new DragWidget(this)),
      m_itemManager(DockItemManager::instance()),
      m_settings(DockSettings::instance()),
      m_xcbMisc(XcbMisc::instance()),
      m_fakeWidget(new QWidget(nullptr))
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_mainPanel);
    setLayout(layout);

    m_dragWidget->setMouseTracking(true);
    m_dragWidget->setFocusPolicy(Qt::NoFocus);
    m_dragWidget->raise();

    m_dragWidget->setCursor(Qt::SizeVerCursor);
    // m_dragWidget->setCursor(Qt::SizeHorCursor);

    m_fakeWidget->setWindowFlags(Qt::FramelessWindowHint);

//    m_xcbMisc->set_window_type(winId(), XcbMisc::Dock);

    for (auto item : m_itemManager->itemList())
        m_mainPanel->insertItem(-1, item);

    initSize();
    resizeMainPanelWindow();

    // blur
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_TranslucentBackground);

    KWindowSystem::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
    XcbMisc::instance()->enableBlurBehind(winId(), true);
    KWindowSystem::setType(winId(), NET::Dock);
    // KWindowEffects::slideWindow(winId(), KWindowEffects::BottomEdge);

    connect(m_dragWidget, &DragWidget::dragPointOffset, this, &MainWindow::onMainWindowSizeChanged);
    connect(m_dragWidget, &DragWidget::dragFinished, this, &MainWindow::onDragFinished);

    connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::initSize, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initSize()
{
    QRect windowRect = m_settings->windowRect();

    // set size.
    QWidget::move(windowRect.left(), windowRect.top());
    QWidget::setFixedSize(windowRect.size());

    m_size = windowRect.size();

    resizeMainPanelWindow();

    setStrutPartial();
}

void MainWindow::setStrutPartial()
{
    // 不清真的作法，kwin设置blur后设置程序支撑导致模糊无效
    QRect rect(geometry());
    rect.setHeight(1);
    rect.setWidth(1);
    m_fakeWidget->setGeometry(rect);
    m_fakeWidget->setVisible(true);
    KWindowSystem::setState(m_fakeWidget->winId(), NET::SkipTaskbar);

    const auto ratio = devicePixelRatioF();
    const QRect windowRect = m_settings->windowRect();
    m_xcbMisc->clear_strut_partial(m_fakeWidget->winId());
    m_xcbMisc->set_strut_partial(m_fakeWidget->winId(), XcbMisc::OrientationBottom, windowRect.height() * ratio + MAINWINDOW_PADDING, windowRect.top(), windowRect.bottom());
}

void MainWindow::resizeMainPanelWindow()
{
    m_dragWidget->setGeometry(0, 0, width(), DRAG_AREA_SIZE);
}

void MainWindow::onMainWindowSizeChanged(QPoint offset)
{
    setFixedHeight(qBound(MAINWINDOW_MIN_SIZE, m_size.height() - offset.y(), MAINWINDOW_MAX_SIZE));
    setFixedWidth(width());

    m_settings->setWindowSize(QWidget::size());
    QRect windowRect = m_settings->windowRect();
    QWidget::move(windowRect.left(), windowRect.top());

    // bottom
    m_dragWidget->setGeometry(0, 0, width(), DRAG_AREA_SIZE);

    resizeMainPanelWindow();
}

void MainWindow::onDragFinished()
{
    m_size = m_settings->windowSize();

    setStrutPartial();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    XcbMisc::instance()->enableBlurBehind(winId(), true);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    QColor color("#EEEEEE");
    color.setAlpha(80);
    painter.fillRect(rect(), color);
}
