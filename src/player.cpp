#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "player.h"

Player::Player() : QGraphicsItem()
{
    this->pwidth  = 35;
    this->pheight = 130;
}

QRectF Player::boundingRect() const
{
    return QRectF( 0, 0, pwidth, pheight );
}

void Player::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    painter->setBrush( Qt::white );
    painter->drawRect( 0, 0, pwidth, pheight );

}


void Player::todown(){}
void Player::toup(){}
