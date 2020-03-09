#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QWidget>
#include <QLabel>

#include <pulse/pulseaudio.h>
#include <pulse/volume.h>
#include <pulse/context.h>

class VolumeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeWidget(QWidget *parent = nullptr);

private:
    QLabel *m_label;

};

#endif // VOLUMEWIDGET_H
