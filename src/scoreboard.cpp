#include <QTime>
#include <QPainter>
#include <QDebug>

#include "scoreboard.h"

ScoreBoard::ScoreBoard( QWidget * parent ) :
    QWidget( parent )
{
}

ScoreBoard::~ScoreBoard()
{
}

void ScoreBoard::setTime( int seconds )
{
    QTime elapsed = QTime( 0, 0, seconds );
    qDebug() << elapsed.minute() << " : " << elapsed.second();
}
