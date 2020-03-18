#include "mainwindow.h"
#include "utils/utils.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>

#include <KF5/KWindowSystem/KWindowEffects>

#define MAINWINDOW_MAX_SIZE       100
#define MAINWINDOW_MIN_SIZE       (45)
#define MAINWINDOW_PADDING (0)
#define DRAG_AREA_SIZE (5)

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
    m_dragWidget->raise();

    m_fakeWidget->setFocusPolicy(Qt::NoFocus);
    m_fakeWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_fakeWidget->setAttribute(Qt::WA_TranslucentBackground);

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
    connect(m_settings, &DockSettings::positionChanged, this, &MainWindow::onPositionChanged);
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

    NETExtendedStrut strut;

    switch (m_settings->position()) {
    case DockSettings::Left:
        strut.left_width = width();
        strut.left_start = y();
        strut.left_end = y() + height() - 1;
        break;
    case DockSettings::Right:
        strut.right_width = width();
        strut.right_start = y();
        strut.right_end = y() + height() - 1;
        break;
    case DockSettings::Bottom:
        strut.bottom_width = height();
        strut.bottom_start = x();
        strut.bottom_end = x() + width();
        break;
    }

    KWindowSystem::setExtendedStrut(m_fakeWidget->winId(),
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
}

void MainWindow::resizeMainPanelWindow()
{
    if (m_settings->position() == DockSettings::Bottom) {
        m_dragWidget->setGeometry(0, 0, width(), DRAG_AREA_SIZE);
        m_dragWidget->setCursor(Qt::SizeVerCursor);
    } else if (m_settings->position() == DockSettings::Left) {
        m_dragWidget->setGeometry(width() - DRAG_AREA_SIZE, 0, DRAG_AREA_SIZE, height());
        m_dragWidget->setCursor(Qt::SizeHorCursor);
    } else {
        m_dragWidget->setGeometry(0, 0, DRAG_AREA_SIZE, height());
        m_dragWidget->setCursor(Qt::SizeHorCursor);
    }
}

void MainWindow::onMainWindowSizeChanged(QPoint offset)
{
    switch (m_settings->position()) {
    case DockSettings::Left:
    case DockSettings::Right:
        setFixedHeight(height());
        setFixedWidth(qBound(MAINWINDOW_MIN_SIZE, m_size.width() + offset.x(), MAINWINDOW_MAX_SIZE));
        m_settings->setWindowLeftSize(QWidget::size());
        break;
    case DockSettings::Bottom:
        setFixedHeight(qBound(MAINWINDOW_MIN_SIZE, m_size.height() - offset.y(), MAINWINDOW_MAX_SIZE));
        setFixedWidth(width());
        m_settings->setWindowBottomSize(QWidget::size());
        break;
    }

    QRect windowRect = m_settings->windowRect();
    QWidget::move(windowRect.left(), windowRect.top());

    resizeMainPanelWindow();
}

void MainWindow::onDragFinished()
{
    m_size = m_settings->windowSize();


    setStrutPartial();
}

void MainWindow::onPositionChanged()
{
    initSize();
    resizeMainPanelWindow();
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
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QColor color("#000000");
    color.setAlpha(60);
    painter.fillRect(rect(), color);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    e->ignore();

    if (e->button() == Qt::RightButton) {
        m_settings->showSettingsMenu();
    }
}
