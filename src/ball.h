#ifndef BALL_H
#define BALL_H

#include <QGraphicsItem>

/**
 * @class Ball ball.h "ball.h"
 * Representa a bola do jogo.
 *
 * Essa classe é responsável por controlar a bola do jogo, calcular o movimento,
 * verificar colisões com as laterais e fundo do campo, etc.
 */
class Ball : public QGraphicsItem
{
public:
    Ball();

    QRectF boundingRect() const;
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget );

    void accelerate();
    void deaccelerate();

protected:
    void advance( int phase );

private:
    qreal angle;
    int   speed;
    int   radius;

    void hitLeftWall();
    void hitRightWall();
    void hitTopWall();
    void hitBottomWall();
    void normalizeAngle();
};

#endif // BALL_H
