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
    angle  = 1.75 * M_PI;   // saída aleatória apenas para testes
    //angle  = (qrand() % 2) ? 0 : M_PI;   // saída para direita ou esquerda
    speed  = 2.5;
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

    normalizeAngle();

    //   TODO
    // 1     CALCULAR POSIÇÃO DE DESTINO |/
    // 2     VERIFICAR SE ESTÁ DENTRO DOS LIMITIES |/
    //  2.1    SE NÃO ESTIVER, ALTERAR |/
    // 3     VERIFICAR COLISÕES NAS PAREDES
    //  3.1    SE FOR NA REGIÃO DO GOL, ENTÃO É GOL E FIM
    // 4     VERIFICAR COLISÕES NOS JOGADORES
    // 5     MOVER PARA A NOVA POSIÇÃO

    QRectF rect = scene()->sceneRect();

    // verifica se a bola bateu em alguma parede
    if ( x() - radius <= rect.left()   ) hitLeftWall();
    if ( y() - radius <= rect.top()    ) hitTopWall();
    if ( x() + radius >= rect.right()  ) hitRightWall();
    if ( y() + radius >= rect.bottom() ) hitBottomWall();

    moveBy( cos( angle ) * speed, -sin( angle ) * speed );
}

/**
 * Método que trata a colisão da bola com uma parede, calculando o ângulo de
 * retorno.
 *
 * @note Esse método não verifica se a bola colidiu em alguma parede, apenas
 * altera seu estado como se tivesse colidido com uma.
 */
void Ball::hitLeftWall()
{
    // move de volta pra dentro do campo para depois não "bater" de novo
    setX(scene()->sceneRect().left() + radius + 0.1);
    // chega entre 90 e 180
//    if (angle > 0.5 * M_PI && angle < M_PI) {
        angle = M_PI - angle;
 //   }
    // entre 180 e 270
//    else {
 //       angle = M_PI - angle;
  //  }
}
void Ball::hitRightWall()
{
    // move de volta pra dentro do campo para depois não "bater" de novo
    setX(scene()->sceneRect().right() - radius - 0.1);
    // chega entre 0 e 90 (subindo)
//    if (angle > 0 && angle < 0.5 * M_PI) {
        angle = M_PI - angle;
//    }
    // entre 270 e 360 (descendo)
//    else {
//        angle = M_PI - angle;
//    }
}
void Ball::hitTopWall()
{
    // move de volta pra dentro do campo para depois não "bater" de novo
    setY(scene()->sceneRect().top() + radius + 0.1);
    angle = 2 * M_PI - angle;
}
void Ball::hitBottomWall()
{
    // move de volta pra dentro do campo para depois não "bater" de novo
    setY(scene()->sceneRect().bottom() - radius - 0.1);
    angle = 2 * M_PI - angle;
}

void Ball::normalizeAngle()
{
    if ( angle < 0 || angle > 2 * M_PI ) {
        angle = qAbs( 2 * M_PI - qAbs( angle ) );
    }
}

/**
 * Método utilizado para acelerar a bola até um determinado nível máximo.
 * O limite máximo para a velocidade é 15 pixels/frame.
 */
void Ball::accelerate()
{
    // velocidade máxima 20
    if ( speed < 20 ) {
        speed += 0.5;
    }
}

/**
 * Método utilizado para desacelerar a bola até um determinado nível mínimo.
 * O limite mínimo para a velocidade é 0.5 pixels/frame.
 */
void Ball::deaccelerate()
{
    // velocidade mínima 0.5
    if ( speed > 0.5 ) {
        speed -= 0.5;
    }
}
