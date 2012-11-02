#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>

#include <cmath>

#include "player.h"
#include "ball.h"

/**
 * Cria uma nova bola.
 *
 * As propriedades são inicializadas com valores pré-definidos.  A bola padrão
 * possui raio de 15 pixels e velocidade 6. O ângulo inicial é randômico, e a
 * bola sempre começa saindo apenas para a direita ou para a esquerda.
 *
 * A bola será mantida dentro do campo @a field passado como parâmetro,
 * rebatendo nas suas paredes conforme as leis da reflexão.
 *
 * @param field O campo em que a bola deve ser jogada.
 *
 * @note Para testes é mantido um ângulo de saída completamente aleatório.
 */
Ball::Ball( QRectF field ) : QGraphicsItem()
{
    this->radius = 15;
    this->angle  = ((qrand() % 200) / 100.0) * M_PI;   // saída aleatória apenas para testes
    //angle  = (qrand() % 2) ? 0 : M_PI;   // saída para direita ou esquerda
    this->speed  = 6;
    this->field = field;
    this->rotationDir = 1;
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
QRectF Ball::boundingRect() const
{
    return QRectF( -this->radius, -this->radius, this->radius * 2, this->radius * 2 );
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


void Ball::paint( QPainter * painter, const QStyleOptionGraphicsItem * style, QWidget * widget )
{
    painter->drawPixmap( this->boundingRect().toRect(), QPixmap( ":/ball.png" ) );
}
/**
 * Muda a direção da bolinha ao colidir com o jogador
 */
void Ball::setAngle(int i){
    // 71 == colisao traseira
    // 72 == colisao superior
    // 73 == colisao inferior
    // else == colisao frontal
    if ( i == 73 ){
        this->moveBy(0, +10);
        angle = 1.25*M_PI;
    }
    else if ( i == 72 ){
        this->moveBy(0, -10);
        angle = 0.75*M_PI;
    }
    else if ( i == 71 ){
        angle = M_PI - angle;
    }
    else {
        angle=(M_PI/180)*(i)*(-0.7);
    }
}

/**
 * Retorna o angulo da bolinha para verificar o lado da colisao
 */
float Ball::getAngle (){
    return this->angle;
}

/**
 * Método utilizado para mover a bola.
 * É chamado automaticamente pelo Qt ao avançar um quadro na animação.
 * Todo o cálculo do movimento da bola e verificação de colisões é feito nesse
 * método ou através de chamadas a outros métodos privados.
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

    // move a bola considerando o ângulo e a velocidade atuais
    moveBy( cos( angle ) * speed, -sin( angle ) * speed );

    // verifica se a bola bateu em alguma parede
    //   !! essas verificações são feitas depois de mover
    //   !! e valem apenas para o próximo frame
    if ( x() - radius < this->field.left()   ) hitLeftWall();
    if ( y() - radius < this->field.top()    ) hitTopWall();
    if ( x() + radius > this->field.right()  ) hitRightWall();
    if ( y() + radius > this->field.bottom() ) hitBottomWall();
}

/**
 * Trata a colisão da bola com a parede esquerda.
 * Esse método calcula o ângulo de retorno da bola e a reposiciona na borda, de
 * modo que não atravesse alguns pixels para fora do campo.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitLeftWall()
{
    setX( this->field.left() + radius );
    angle = M_PI - angle;
    this->rotationDir = cos( this->angle ) >= 0 ? 1 : -1;
}

/**
 * Trata a colisão da bola com a parede direita.
 * Esse método calcula o ângulo de retorno da bola e a reposiciona na borda, de
 * modo que não atravesse alguns pixels para fora do campo.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitRightWall()
{
    setX( this->field.right() - radius );
    angle = M_PI - angle;
    this->rotationDir = cos( this->angle ) >= 0 ? 1 : -1;
}

/**
 * Trata a colisão da bola com a parede superior.
 * Esse método calcula o ângulo de retorno da bola e a reposiciona na borda, de
 * modo que não atravesse alguns pixels para fora do campo.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitTopWall()
{
    setY( this->field.top() + radius );
    angle = 2 * M_PI - angle;
    this->rotationDir = cos( this->angle ) >= 0 ? 1 : -1;
}

/**
 * Trata a colisão da bola com a parede inferior.
 * Esse método calcula o ângulo de retorno da bola e a reposiciona na borda, de
 * modo que não atravesse alguns pixels para fora do campo.
 *
 * @note Esse método não verifica se a bola colidiu na parede, apenas altera seu
 *       estado como se tivesse colidido com ela.
 */
void Ball::hitBottomWall()
{
    setY( this->field.bottom() - radius );
    angle = 2 * M_PI - angle;
    this->rotationDir = cos( this->angle ) >= 0 ? 1 : -1;
}

/**
 * Método auxiliar para normalizar o ângulo.
 * O objetivo desse método é manter o ângulo de movimentação da da bola sempre
 * dentro da faixa de 0 a 360 graus.
 * Isso é necessário para o cálculo correto das colisões da bola com as parede.
 */
void Ball::normalizeAngle()
{
    while ( angle < 0 || angle > 2 * M_PI ) {
        angle = qAbs( 2 * M_PI - qAbs( angle ) );
    }
}

/**
 * Método utilizado para acelerar a bola.
 * O limite máximo estabelecido para a velocidade da bola é de 25 pixels/frame.
 *
 * @note A velocidade real da bola também depende do número de frames por
 *       segundo (FPS) do jogo.
 */
void Ball::accelerate()
{
    if ( speed <= 24 ) {
        speed++;
    }
}

/**
 * Método utilizado para desacelerar a bola.
 * O limite mínimo estabelecido para a velocidade da bola é de 1 pixel/frame.
 *
 * @note A velocidade real da bola também depende do número de frames por
 *       segundo (FPS) do jogo.
 */
void Ball::deaccelerate()
{
    if ( speed >= 2 ) {
        speed--;
    }
}

void Ball::rotate()
{
    this->setRotation( this->rotation() + ( speed * 2 * this->rotationDir ) );
}

void Ball::rotate( bool reverse )
{
    this->setRotation( this->rotation() + ( speed * 2 * ( reverse ? -1 : 1 ) ) );
}

short int Ball::getReversed()
{
    return this->rotationDir == 1 ? 0 : 1;
}
