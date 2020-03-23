#ifndef DOCKITEM_H
#define DOCKITEM_H

#include <QWidget>
#include <QPointer>
#include <QMenu>
#include <QVariantAnimation>

class DockItem : public QWidget
{
    Q_OBJECT

public:
    enum ItemType {
        Fixed,
        App
    };

    explicit DockItem(QWidget *parent = nullptr);

    inline virtual ItemType itemType() const { Q_UNREACHABLE(); return App; }

    QSize sizeHint() const override;

    virtual const QString contextMenu() const;
    const QRect perfectIconRect() const;

    virtual QWidget *popupTips();

    bool isHover() { return m_hover; };

private:
    void showContextMenu();
    const QPoint popupMarkPoint() const;
    const QPoint topleftPoint() const;

protected:
    void paintEvent(QPaintEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QVariantAnimation *m_hoverAnimation;
    QTimer *m_popupTipsDelayTimer;
    QMenu m_contextMenu;
    qreal m_hoverSize;
    bool m_hover;
};

#endif // DOCKITEM_H
