#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#define FONT_NAME "Erbos Draco 1st Open NBP"

#include <QGraphicsItem>

class QTime;

/**
 * @class ScoreBoard scoreboard.h "scoreboard.h"
 * Representa o placar do jogo.
 *
 * O placar exibe informações sobre o número de gols de cada jogador e o tempo
 * de jogo.
 */
class ScoreBoard : public QGraphicsItem
{
public:
    ScoreBoard();

    QRectF boundingRect() const;
    void paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget );

    void setTime( int seconds );
    void setLeftPlayerName( QString name );
    void setRightPlayerName( QString name );
    void setLeftScore( int goals );
    void setRightScore( int goals );

private:
    int leftScore;
    int rightScore;
    QTime elapsed;
    QString leftPlayerName;
    QString rightPlayerName;
};

#endif // SCOREBOARD_H
