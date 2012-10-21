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
    //this->grabKeyboard();
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

void Player::todown()
{
    if (LEFT == mode)
    {
        if (initpos<425){
            initpos++;
        }
        moveBy(20, initpos);
    }
    else if (RIGHT == mode)
    {
        if (initpos<425){
            initpos++;
        }
        moveBy(945, initpos);
    }
}

void Player::keyPressEvent( QKeyEvent * event )
{
    if ( Qt::Key_Up == event->key() ) {
        event->accept();
        this->toup();
        //qDebug() << "Pra cima";
    }
    else if ( Qt::Key_Down == event->key() ) {
        event->accept();
        //qDebug() << "Pra baixo";
    }
}

void Player::toup()
{
    if (LEFT == mode)
    {
        if (initpos<75){
            initpos=initpos-5;
        }
        moveBy(20, initpos);
    }
    else if (RIGHT == mode)
    {
        if (initpos<75){
            initpos--;
        }
        moveBy(945, initpos);
    }
}
