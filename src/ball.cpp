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
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#boundingRect
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
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#paint
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
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#advance
 */
void Ball::advance( int phase )
{
    // Fase de preparação para se mover. Não precisamos fazer nada, então sai.
    if ( !phase ) {
        return;
    }

    // garante um ângulo sempre entre 0 e 360 graus
    normalizeAngle();

    QRectF rect = scene()->sceneRect();

    // move a bola considerando o ângulo e a velocidade atuais
    // o movimento é feito em números inteiros (facilita a comunicação serial)
    moveBy( qRound( cos( angle ) * speed ), qRound( -sin( angle ) * speed ) );

    // verifica se a bola bateu em alguma parede
    //   !! essas verificações são feitas depois de mover
    //   !! e valem apenas para o próximo frame
    if ( x() - radius <= rect.left()   ) hitLeftWall();
    if ( y() - radius <= rect.top()    ) hitTopWall();
    if ( x() + radius >= rect.right()  ) hitRightWall();
    if ( y() + radius >= rect.bottom() ) hitBottomWall();

    qDebug() << x() << " x " << y();
}

/**
 * Método que trata a colisão da bola com a parede esquerda, calculando o ângulo
 * de retorno.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitLeftWall()
{
    // move sempre para o borda (evita que atravesse um ou dois pixels)
    setX(scene()->sceneRect().left() + radius);

    angle = M_PI - angle;
}

/**
 * Método que trata a colisão da bola com a parede direita, calculando o ângulo
 * de retorno.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitRightWall()
{
    // move sempre para o borda (evita que atravesse um ou dois pixels)
    setX(scene()->sceneRect().right() - radius);

    angle = M_PI - angle;
}

/**
 * Método que trata a colisão da bola com a parede superior, calculando o ângulo
 * de retorno.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitTopWall()
{
    // move sempre para o borda (evita que atravesse um ou dois pixels)
    setY(scene()->sceneRect().top() + radius);

    angle = 2 * M_PI - angle;
}

/**
 * Método que trata a colisão da bola com a parede inferior, calculando o ângulo
 * de retorno.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitBottomWall()
{
    // move sempre para o borda (evita que atravesse um ou dois pixels)
    setY(scene()->sceneRect().bottom() - radius);

    angle = 2 * M_PI - angle;
}

/**
 * Método auxiliar para normalizar o ângulo e mantê-lo sempre dentro da faixa
 * entre 0 e 360 graus.
 */
void Ball::normalizeAngle()
{
    if ( angle < 0 || angle > 2 * M_PI ) {
        angle = qAbs( 2 * M_PI - qAbs( angle ) );
    }
}

/**
 * Método utilizado para acelerar a bola até um determinado nível máximo.
 * O limite máximo para a velocidade é 20 pixels/frame.
 *
 * @note A velocidade real da bola também depende do número de frames por
 *       segundo (FPS) do jogo.
 */
void Ball::accelerate()
{
    // velocidade máxima 20
    if ( speed <= 19 ) {
        speed++;
    }
}

/**
 * Método utilizado para desacelerar a bola até um determinado nível mínimo.
 * O limite mínimo para a velocidade é 0.5 pixels/frame.
 *
 * @note A velocidade real da bola também depende do número de frames por
 *       segundo (FPS) do jogo.
 */
void Ball::deaccelerate()
{
    // velocidade mínima 1
    if ( speed >= 2 ) {
        speed--;
    }
}
