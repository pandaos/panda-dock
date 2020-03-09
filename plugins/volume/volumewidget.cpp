#include "volumewidget.h"
#include <QVBoxLayout>
#include <QDebug>

VolumeWidget::VolumeWidget(QWidget *parent)
    : QWidget(parent),
      m_label(new QLabel("100%"))
{

}
