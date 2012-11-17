#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

#include "QKeyEvent"
#include "gameoptions.h"
#include "player.h"


/**
 * Cria os jogadores.
 *
 * As propriedades são inicializadas com valores pré-definidos.  Dimensao 130x35 pixel.
 *
 */
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

/**
 * Define a partir da opcao de jogo (servidor ou cliente) qual a imagem será carregada no jogador
 *
 */
void Player::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    if ( LEFT == mode ){
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/left-player.png" ) );
    }
    else{
        painter->drawPixmap( 0, 0, pwidth, pheight, QPixmap( ":/right-player.png" ) );
    }
}

/**
 * Movimentação do jogador pelas teclas escolhidas
 *
 * Desloca a partir da posição inicial 10 pixels para cima
 *
 */

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

/**
 * Movimentação do jogador pelas teclas escolhidas
 *
 * Desloca a partir da posição inicial 10 pixels para baixo
 *
 */

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
        if (y() <= 360){
            moveBy(0, +10);
        }
    }
}
