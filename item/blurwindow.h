#ifndef BLURWINDOW_H
#define BLURWINDOW_H

#include <QWidget>

class QLabel;
class BlurWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlurWindow(QWidget *parent = nullptr);

    void setText(const QString &text);
    void update();

private:
    void paintEvent(QPaintEvent *e);

private:
    QLabel *m_textLabel;
};

#endif // BLURWINDOW_H
