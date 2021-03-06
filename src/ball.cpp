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
    this->angle  = (qrand() % 2) ? 0 : M_PI;   // saída para direita ou esquerda
    this->speed  = 6;
    this->field  = field;

    // por padrão considera todo o fundo do campo como o gol.
    // ver método Ball::setGoals
    this->limitGoalTop    = field.top();
    this->limitGoalBottom = field.bottom();
    this->goalWidth       = 0;
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
void Ball::setAngle(int i, int j){

    // 71 == colisao traseira
    // 72 == colisao superior
    // 73 == colisao inferior
    // else == colisao frontal

    //player 1
    if ( j== 1){
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

    //player 2
    if ( j==2 ) {
        if ( i == 73 ){
            this->moveBy(0, +10);
            angle = 1.75*M_PI;
        }
        else if ( i == 72 ){
            this->moveBy(0, -10);
            angle = 0.25*M_PI;
        }
        else if ( i == 71 ){
            angle = M_PI - angle;
        }
        else {
            if ( i>0 && i<70 ) {
                angle = ( ( M_PI/180 ) * ( i ) * ( 0.3 ) ) + M_PI;
            }
            else if ( i<0 && i>-70 ) {
                angle = ( ( M_PI/180 ) * ( i ) * ( 0.3 ) ) - M_PI;
            }
            else {
                angle = M_PI - angle;
            }
        }
    }
}

/**
 * Retorna o angulo da bolinha para verificar o lado da colisao
 */
float Ball::getAngle (){
    this->normalizeAngle();
    return this->angle;
}

/**
 * Método utilizado para mover a bola.
 *
 * É chamado automaticamente pelo Qt ao avançar um quadro na animação.
 *
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
    this->normalizeAngle();

    // move a bola considerando o ângulo e a velocidade atuais
    this->moveBy( cos( this->angle ) * this->speed, -sin( this->angle ) * this->speed );

    // rotação
    this->setRotation( this->rotation() + ( this->speed * 2 * cos( this->angle ) ) );

    // verifica se a bola bateu em alguma parede
    //   !! essas verificações são feitas depois de mover
    //   !! e valem apenas para o próximo frame

    bool goalArea = y() - radius > limitGoalTop && y() + radius < limitGoalBottom;

    // colisões nas laterais e fundo do campo
    if ( x() - radius < this->field.left()   && !goalArea ) hitLeftWall();
    if ( y() - radius < this->field.top()    && !goalArea ) hitTopWall();
    if ( x() + radius > this->field.right()  && !goalArea ) hitRightWall();
    if ( y() + radius > this->field.bottom() && !goalArea ) hitBottomWall();
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
    this->setX( this->field.left() + this->radius );
    this->angle = M_PI - this->angle;
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
    this->setX( this->field.right() - this->radius );
    this->angle = M_PI - this->angle;
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
    this->setY( this->field.top() + this->radius );
    this->angle = 2 * M_PI - this->angle;
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
    this->setY( this->field.bottom() - this->radius );
    this->angle = 2 * M_PI - this->angle;
}

/**
 * Método auxiliar para normalizar o ângulo.
 * O objetivo desse método é manter o ângulo de movimentação da da bola sempre
 * dentro da faixa de 0 a 360 graus.
 * Isso é necessário para o cálculo correto das colisões da bola com as parede.
 */
void Ball::normalizeAngle()
{
    while ( this->angle < 0 || this->angle > 2 * M_PI ) {
        this->angle = qAbs( 2 * M_PI - qAbs( this->angle ) );
    }
}

/**
 * Método utilizado para definir a velocidade de deslocamento da bola.
 *
 * O limite mínimo para a velocidade da bola é de 1 pixel/frame, e o máximo é de
 * 20 pixels/frame.
 *
 * @param speed A velocidade que deve ser definida para a bola.
 *
 * @note A velocidade real da bola também depende do número de frames por
 *       segundo (FPS) do jogo.
 */
void Ball::setSpeed( int speed )
{
    if ( speed >= 1 && speed <= 20 ) {
        this->speed = speed;
    }
}

/**
 * Obtém a velocidade da bola.
 * @return A velocidade atual da bola.
 */
int Ball::getSpeed()
{
    return this->speed;
}

/**
 * Redefine o ângulo de deslocamento da bola para o movimento na horizontal.
 *
 * O ângulo de deslocamento é redefinido para o movimento inicial, ou seja, a
 * bola será movida para a direita ou para a esquerda. A direção será definida
 * para o inverso da atual: se a bola estava se deslocando para a esquerda (com
 * ângulo entre 90 e 270 graus), então seu deslocamento será revertido para a
 * direita.
 *
 * @note Esse método é utilizado após ser detectado um gol no jogo.
 * @see Game::verifyGoal()
 */
void Ball::resetAngle()
{
    this->normalizeAngle();
    this->angle = ( this->angle > 0.5 * M_PI && this->angle <= 1.5 * M_PI ) ? 0 : M_PI;
}

/**
 * Define a posição das goleiras no campo.
 *
 * É necessário saber a localização das goleiras no campo para que a bola não
 * rebata nelas como se fossem as laterais ou o fundo do campo. E também para
 * que a bola possa ter o efeito de "entrar" no gol.
 *
 * @param top    O limite superior das goleiras.
 * @param bottom O limite inferior das goleiras.
 * @param width  A largura/profundidade da goleira
 */
void Ball::setGoals( int top, int bottom, int width )
{
    this->limitGoalTop    = top;
    this->limitGoalBottom = bottom;
    this->goalWidth       = width;
}
