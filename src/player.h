#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsItem>


class QKeyEvent;

/**
 * @class Player player.h "player.h"
 * Representa os jogadores.
 *
 * Essa classe é responsável por controlar o jogador e definir qual imagem será
 * carregada para cada um (servidor e cliente).
 */

class Player : public QGraphicsItem
{
public:
    enum PlayerMode {
        LEFT,
        RIGHT
    };

    Player(PlayerMode mode);

    QRectF boundingRect() const;
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget );

    Player::PlayerMode getPlayerMode();

    void todown();
    void toup();

private:
    int  pwidth;
    int  pheight;
    float initpos;
    PlayerMode mode;
};

#endif // PLAYER_H
