#ifndef BALL_H
#define BALL_H

#include <QGraphicsItem>

class Ball : public QGraphicsItem
{
public:
    Ball();

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *);

    void accelerate();
    void deaccelerate();

protected:
    void advance( int phase );

private:
    qreal angle;
    qreal speed;
    qreal radius;

    void hitLeftWall();
    void hitRightWall();
    void hitTopWall();
    void hitBottomWall();
};

#endif // BALL_H
