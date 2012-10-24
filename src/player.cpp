#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "QKeyEvent"
#include "gameoptions.h"
#include "player.h"

Player::Player(PlayerMode mode) : QGraphicsItem()
{
    this->pwidth  = 35;
    this->pheight = 130;
    this->mode = mode;
    this->initpos = 185;
}

QRectF Player::boundingRect() const
{
    return QRectF( 0, 0, pwidth, pheight );
}

void Player::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    if ( LEFT == mode ){
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/left-player.png" ) );
    }
    else{
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/right-player.png" ) );
    }
}

void Player::toup()
{
    if (LEFT == mode)
    {
        if (y() >= 10){
            moveBy(0, -10);
        }

    }
    else if (RIGHT == mode)
    {
        if (y() >= 10){
            moveBy(0, -10);
        }
    }
}

void Player::todown()
{
    if (LEFT == mode)
    {
        if (y() <= 360){
            moveBy(0, +10);
        }
    }
    else if (RIGHT == mode)
    {
        if (y() >= 360){
            moveBy(0, +10);
        }
    }
}
