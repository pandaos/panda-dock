#include <QPixmap>
#include <QImageReader>
#include <QApplication>
#include <QScreen>
#include <QIcon>

namespace Utils
{
    static QPixmap renderSVG(const QString &path, const QSize &size) {
        const qreal devicePixelRatio = qApp->devicePixelRatio();
        QImageReader reader;
        QPixmap pixmap;
        reader.setFileName(path);
        if (reader.canRead()) {
            reader.setScaledSize(size * devicePixelRatio);
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
        else {
            pixmap.load(path);
        }

        return pixmap;
    }

    static const QPixmap getIcon(const QString iconName, const int size, const qreal ratio)
    {
        QPixmap pixmap;

        // 把size改为小于size的最大偶数 :)
        const int s = int(size * ratio) & ~1;

        // load pixmap from Icon-Theme.
        const QIcon icon = QIcon::fromTheme(iconName);
        const int fakeSize = std::max(48, s); // cannot use 16x16, cause 16x16 is label icon
        pixmap = icon.pixmap(QSize(fakeSize, fakeSize));

        if (pixmap.isNull()) {
            pixmap = QIcon::fromTheme("application-x-desktop").pixmap(QSize(fakeSize, fakeSize));
        }

        if (!pixmap.isNull()) {
            if (pixmap.size().width() != s) {
                pixmap = pixmap.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            pixmap.setDevicePixelRatio(ratio);
        }

        return pixmap;
    }

}
