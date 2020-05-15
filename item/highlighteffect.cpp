#include "highlighteffect.h"
#include <QPainter>
#include <QDebug>

static QPixmap lighterEffect(const QPixmap pixmap, const int delta)
{
    QImage image = pixmap.toImage();

    const int width = image.width();
    const int height = image.height();
    const int bytesPerPixel = image.bytesPerLine() / image.width();

    for (int i = 0; i != height; ++i) {
        uchar *scanLine = image.scanLine(i);
        for (int j = 0; j != width; ++j) {
            QRgb &rgba = *(QRgb*)scanLine;
            if (qAlpha(rgba) == 0xff) {
                rgba = QColor::fromRgba(rgba).light(delta).rgba();
            }
            scanLine += bytesPerPixel;
        }
    }

    return QPixmap::fromImage(image);
}

HighlightEffect::HighlightEffect(QObject *parent)
    : QGraphicsEffect(parent)
    , m_highlighting(false)
{

}

void HighlightEffect::draw(QPainter *painter)
{
    const QPixmap pix = sourcePixmap(Qt::DeviceCoordinates);

    if (m_highlighting) {
        painter->drawPixmap(0, 0, lighterEffect(pix, 120));
    } else {
        painter->drawPixmap(0, 0, pix);
    }
}