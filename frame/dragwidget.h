#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QWidget>

class DragWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DragWidget(QWidget *parent = nullptr);

signals:
    void dragPointOffset(QPoint);
    void dragFinished();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    bool m_dragStatus;
    QPoint m_resizePoint;
};

#endif // DRAGWIDGET_H
