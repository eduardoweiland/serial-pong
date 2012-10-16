#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "player.h"

Player::Player(QObject *parent) :
    QGraphicsItem(parent)
{
    pwidth=35;
    pheight=130;

}



void Player::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    painter->setBrush( Qt::white );
    painter->drawRect( pwidth, pheight );

}



