#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsItem>

class Player : public QGraphicsItem
{
public:
    Player();

    QRectF boundingRect() const;
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget );

    void todown();
    void toup();

private:
    int  pwidth;
    int  pheight;

};

#endif // PLAYER_H
 * @class Player player.h "player.h"
