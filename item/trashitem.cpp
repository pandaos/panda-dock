#include "trashitem.h"
#include "utils/utils.h"
#include <QProcess>
#include <QPainter>
#include <QDir>

const QString TrashDir = QDir::homePath() + "/.local/share/Trash";
const QDir::Filters ItemsShouldCount = QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot;

TrashItem::TrashItem(QWidget *parent)
    : DockItem(parent),
      m_filesWatcher(new QFileSystemWatcher(this))
{
    connect(m_filesWatcher, &QFileSystemWatcher::directoryChanged, this, &TrashItem::onDirectoryChanged, Qt::QueuedConnection);

    setAccessibleName("Trash");
    onDirectoryChanged();
}

void TrashItem::refreshIcon()
{
    const int iconSize = qMin(width(), height());

    if (m_count) {
        m_iconPixmap = Utils::renderSVG(":/resources/user-trash-full.svg", QSize(iconSize * 0.8, iconSize * 0.8));
    } else {
        m_iconPixmap = Utils::renderSVG(":/resources/user-trash.svg", QSize(iconSize * 0.8, iconSize * 0.8));
    }

    QWidget::update();
}

void TrashItem::onDirectoryChanged()
{
    m_filesWatcher->addPath(TrashDir);

    if (QDir(TrashDir + "/files").exists()) {
        m_filesWatcher->addPath(TrashDir + "/files");
        m_count = QDir(TrashDir + "/files").entryList(ItemsShouldCount).count();
    } else {
        m_count = 0;
    }

    refreshIcon();
}

void TrashItem::paintEvent(QPaintEvent *e)
{
    DockItem::paintEvent(e);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const auto ratio = devicePixelRatioF();
    const int iconX = rect().center().x() - m_iconPixmap.rect().center().x() / ratio;
    const int iconY = rect().center().y() - m_iconPixmap.rect().center().y() / ratio;

    painter.drawPixmap(iconX, iconY, m_iconPixmap);
}

void TrashItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    refreshIcon();
}

void TrashItem::mouseReleaseEvent(QMouseEvent *e)
{
    QProcess::startDetached("gio", QStringList() << "open" << "trash:///");
    QWidget::mouseReleaseEvent(e);
}
