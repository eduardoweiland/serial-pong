#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "player.h"

Player::Player(PlayerMode mode) : QGraphicsItem()
{
    this->pwidth  = 35;
    this->pheight = 130;
    this->mode = mode;
}

QRectF Player::boundingRect() const
{
    return QRectF( 0, 0, pwidth, pheight );
}

void Player::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    //painter->setBrush( Qt::white );
    //painter->drawRect( 0, 0, pwidth, pheight );
    if ( LEFT == mode ){
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/left-player.png" ) );
    }
    else{
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/right-player.png" ) );
    }

}


void Player::todown(){}
void Player::toup(){}
