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
    Ball( QRectF field );

    QRectF boundingRect() const;
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget );

    void accelerate();
    void deaccelerate();
    void rotate();
    void rotate( bool reverse );
    short int getReversed();
    void resetAngle();

    //teste
    void setAngle(int i, int j);
    float getAngle ();
    //teste

protected:
    void advance( int phase );

private:
    qreal     angle;
    int       speed;
    int       radius;
    QRectF    field;
    short int rotationDir;

    void hitLeftWall();
    void hitRightWall();
    void hitTopWall();
    void hitBottomWall();
    void normalizeAngle();
};

#endif // BALL_H
