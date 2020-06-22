#ifndef HIGHLIGHTEFFECT_H
#define HIGHLIGHTEFFECT_H

#include <QGraphicsEffect>

class HighlightEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    explicit HighlightEffect(QObject *parent = nullptr);

    void setHighlighting(const bool highlighting) { m_highlighting = highlighting; }

protected:
    void draw(QPainter *painter) override;

private:
    bool m_highlighting;
};

#endif // HIGHLIGHTEFFECT_H
