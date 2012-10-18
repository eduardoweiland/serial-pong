#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsItem>

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
    PlayerMode mode;
};

#endif // PLAYER_H
