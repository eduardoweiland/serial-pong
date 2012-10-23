#include <QTime>
#include <QPainter>
#include <QDebug>
#include <QGraphicsItem>

#include "scoreboard.h"

/**
 * Cria um novo placar.
 *
 * O placar é responsável apenas por exiber informações como a pontuação dos
 * jogadores e o tempo do jogo.  Esses valores são controlados pela classe Game.
 */
ScoreBoard::ScoreBoard() : QGraphicsItem()
{
    this->leftScore  = 0;
    this->rightScore = 0;
    this->elapsed    = QTime( 0, 0, 0 );
}

/**
 * Retorna a área ocupada por esse objeto.
 *
 * Necessário para que o Qt possa mover corretamente o item, pois essa é a área
 * que será atualizada (redesenhada) na tela.
 *
 * @return A área retangular ocupada pela bola na tela.
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#boundingRect
 */
QRectF ScoreBoard::boundingRect() const
{
    return QRectF( 0, 0, 1000, 100 );
}

/**
 * Método utilizado para desenhar o item na tela.
 *
 * O Qt realiza o trabalho de chamar esse método ao adicioná-lo à cena e ao
 * atualizá-lo através do método QGraphicsItem::update.
 *
 * @param painter Ponteiro para o QPainter usado para desenhar o item.
 * @param style   Não utilizado.
 * @param widget  Não utilizado.
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#paint
 */
void ScoreBoard::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    // imagem de fundo e cor da fonte
    painter->drawPixmap( 0, 0, 1000, 100, QPixmap( ":/scoreboard.png" ) );
    painter->setPen( Qt::yellow );

    // tempo de jogo
    painter->setFont( QFont( "Erbos Draco 1st Open NBP", 17 ) );
    painter->drawText( 456, 67, 36, 25, 0, this->elapsed.toString( "mm" ) );
    painter->drawText( 518, 67, 36, 25, 0, this->elapsed.toString( "ss" ) );

    // placar/número de gols dos jogadores
    int leftDec  = this->leftScore  / 10;
    int leftNum  = this->leftScore  % 10;
    int rightDec = this->rightScore / 10;
    int rightNum = this->rightScore % 10;

    painter->setFont( QFont( "Erbos Draco 1st Open NBP", 40 ) );
    painter->drawText( 387, 5, 85, 60, 0, QString::number( leftDec ) + QString::number( leftNum ) );
    painter->drawText( 532, 5, 85, 60, 0, QString::number( rightDec ) + QString::number( rightNum ) );
}

/**
 * Método que permite definir o tempo de jogo percorrido.
 *
 * Uma atualização na tela será agendada para o próximo frame automaticamente
 * pelo Qt ao chamar o método update. A área redesenhada será apenas onde é
 * exibido o cronômetro de tempo de jogo.
 *
 * @param seconds O número de segundos passados desde o início do jogo.
 * @see Game::play
 * @see ScoreBoard::paint
 */
void ScoreBoard::setTime( int seconds )
{
    this->elapsed = QTime( seconds / 3600, seconds / 60, seconds % 60 );
    // atualiza apenas a área do relógio na tela
    this->update( 456, 67, 100, 25 );
}
