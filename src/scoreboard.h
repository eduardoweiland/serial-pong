#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QWidget>

class QTime;

/**
 * @class ScoreBoard scoreboard.h "scoreboard.h"
 * Representa o placar do jogo.
 *
 * O placar exibe informações sobre o número de gols de cada jogador e o tempo
 * de jogo.
 */
class ScoreBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreBoard( QWidget * parent = 0 );
    ~ScoreBoard();

    void setTime( int seconds );

private:
};

#endif // SCOREBOARD_H
