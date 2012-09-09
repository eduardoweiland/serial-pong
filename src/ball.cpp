//rafaomercenario@hotmail.com
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

#include <cmath>

#include "ball.h"

/**
 * Construtor padrão.
 * Inicializa as propriedades da classe com valores padrão.
 */
Ball::Ball() : QGraphicsItem()
{
    radius = 15;
    angle  = 0.4 * M_PI;   // saída aleatória apenas para testes
    //angle  = (qrand() % 2) ? 0 : M_PI;   // saída para direita ou esquerda
    speed  = 3.5;
}

/**
 * Sobrescreve o método do Qt.
 * Necessário para que o Qt possa mover corretamente o item.
 *
 * @return A área retangular ocupada pela bola na tela.
 */
QRectF Ball::boundingRect() const
{
    return QRectF( -radius, -radius, radius * 2, radius * 2 );
}

/**
 * Método utilizado para desenhar o item na tela.
 * O Qt realiza o trabalho de chamar esse método ao adicioná-lo à cena.
 *
 * @param painter Ponteiro para o QPainter usado para desenhar o item.
 * @param Não utilizado.
 * @param Não utilizado.
 */
void Ball::paint( QPainter * painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    painter->setBrush( Qt::white );
    painter->drawEllipse( -radius, -radius, radius * 2, radius * 2 );
}

/**
 * Método utilizado para mover a bola.
 * É chamado pelo Qt ao avançar um quadro na animação.
 *
 * @param phase Indica se o item deve se mover (1) ou apenas se preparar (0).
 */
void Ball::advance( int phase )
{
    // Fase de preparação para se mover. Não precisamos fazer nada, então sai.
    if ( !phase ) {
        return;
    }

    //   TODO
    // 1     CALCULAR POSIÇÃO DE DESTINO |/
    // 2     VERIFICAR SE ESTÁ DENTRO DOS LIMITIES |/
    //  2.1    SE NÃO ESTIVER, ALTERAR |/
    // 3     VERIFICAR COLISÕES NAS PAREDES
    //  3.1    SE FOR NA REGIÃO DO GOL, ENTÃO É GOL E FIM
    // 4     VERIFICAR COLISÕES NOS JOGADORES
    // 5     MOVER PARA A NOVA POSIÇÃO

    QRectF rect = scene()->sceneRect();
    qreal destX = x() + cos( angle ) * speed;
    qreal destY = y() - sin( angle ) * speed;

    // garante que a bola não sai do campo
    if ( destX - radius < rect.left()   ) destX = rect.left();
    if ( destY - radius < rect.top()    ) destY = rect.top();
    if ( destX + radius > rect.right()  ) destX = rect.right();
    if ( destY + radius > rect.bottom() ) destY = rect.bottom();

    // verifica a posição em que a bola bateu
    int top    = qRound( radius - rect.top()    + y() );
    int bottom = qRound( radius + rect.bottom() - y() );
    int left   = qRound( radius - rect.left()   + x() );
    int right  = qRound( radius + rect.right()  - x() );
    int min    = qMin(  qMin( top, bottom ), qMin( left, right ) );

    // bateu no topo
    if ( min == top ) {
        angle = M_PI - angle;
    }
    else if ( min == bottom ) {
        angle = 2 * M_PI - angle;
    }
    else if ( min == left ) {
        qDebug("left");
    }
    else if (min == right) {
        qDebug("right");
    }

    destX = qRound(destX - x() + cos( angle ) * speed);
    destY = qRound(destY - y() - sin( angle ) * speed);
    setPos( destX, destY );
}

/**
 * Método que trata a colisão da bola com uma parede, calculando o ângulo de
 * retorno.
 *
 * @note Esse método não verifica se a bola colidiu em alguma parede, apenas
 * altera seu estado como se tivesse colidido com uma.
 */
void Ball::collidesWithWall() {
}

/**
 * Método utilizado para acelerar a bola até um determinado nível máximo.
 * O limite máximo para a velocidade é 15 pixels/frame.
 */
void Ball::accelerate() {
    // velocidade máxima 15
    if ( speed < 15 ) {
        speed += 0.5;
    }
}

/**
 * Método utilizado para desacelerar a bola até um determinado nível mínimo.
 * O limite mínimo para a velocidade é 0.5 pixels/frame.
 */
void Ball::deaccelerate() {
    // velocidade mínima 0.5
    if ( speed > 0.5 ) {
        speed -= 0.5;
    }
}
