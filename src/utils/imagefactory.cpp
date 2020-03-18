#include "imagefactory.h"

#include <QDebug>
#include <QPainter>

ImageFactory::ImageFactory(QObject *parent)
    : QObject(parent)
{

}

QPixmap ImageFactory::lighterEffect(const QPixmap pixmap, const int delta)
{
    QImage image = pixmap.toImage();

    const int width = image.width();
    const int height = image.height();
    const int bytesPerPixel = image.bytesPerLine() / image.width();

    for (int i(0); i != height; ++i)
    {
        uchar *scanLine = image.scanLine(i);
        for (int j(0); j != width; ++j)
        {
            QRgb &rgba = *(QRgb*)scanLine;
            if (qAlpha(rgba) == 0xff) {
                rgba = QColor::fromRgba(rgba).light(delta).rgba();
//                const QColor hsv = QColor::fromRgba(rgba).toHsv();
//                // check brightness first, color with max brightness processed with lighter will
//                // become white like color which will ruin the whole image in general cases.
//                if (hsv.value() < 255) {
//                    rgba = QColor::fromRgba(rgba).lighter(delta).rgba();
//                }
            }
            scanLine += bytesPerPixel;
        }
    }

    return QPixmap::fromImage(image);
}
