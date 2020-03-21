#ifndef APPSCROLLAREA_H
#define APPSCROLLAREA_H

#include <QScrollArea>
#include <QBoxLayout>
#include "item/appitem.h"

class AppScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit AppScrollArea(QWidget *parent = nullptr);

    QBoxLayout *layout() { return m_mainLayout; };

    void addItem(AppItem *item);
    void removeItem(AppItem *item);
    void scrollToItem(AppItem *item);

    void setRange(int value);

protected:
    void wheelEvent(QWheelEvent *e) override;

private:
    QWidget *m_mainWidget;
    QBoxLayout *m_mainLayout;
    int m_range;
};

#endif // APPSCROLLAREA_H
