#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <cmath>

#include "ball.h"
#include "game.h"
#include "globals.h"
#include "qextserialport.h"
#include "player.h"
#include "scoreboard.h"

/**
 * Cria um novo jogo.
 *
 * A classe herda de QGraphicsView e representa toda a área do jogo, incluindo
 * o campo, os jogadores, a bola e inclusive o placar.
 *
 * O tamanho do jogo é predefinido em 1000px de largura e 600px de altura.
 *
 * @param parent Ponteiro para o widget pai do jogo.
 *
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsview.html#QGraphicsView
 */
Game::Game( QWidget * parent ) :
    QGraphicsView( new QGraphicsScene( -50, 0, 1100, 600 ), parent )
{
    this->port                = NULL;
    this->portName            = SERIALPORT; // porta padrão COM1 (/dev/ttyS0)
    this->timer               = NULL;
    this->gameTime            = NULL;
    this->gameMode            = UNKNOWN;
    this->moveUpKeyCode       = Qt::Key_Up;
    this->moveDownKeyCode     = Qt::Key_Down;
    this->displayedText       = NULL;
    this->displayedTextEffect = NULL;
    this->otherReady          = false;

    this->initializeConfig();
    qApp->installEventFilter( this );

    // cria a cena para conter todos os itens do jogo.
    this->scene()->setBackgroundBrush( Qt::black );

    QRectF fieldRect = QRectF( 0, 0, 1000, 500 );

    // o campo do jogo (a bola não deve sair dele)
    this->field = new QGraphicsRectItem( fieldRect );
    this->field->setBrush( QPixmap ( ":/background.png" ) );
    this->scene()->addItem( this->field );

    this->goalLeft = new QGraphicsRectItem( 0, 0, 50, 200 );
    this->goalLeft->setPen( QPen( Qt::white ) );
    this->goalLeft->setBrush( Qt::white );
    this->goalLeft->setPos( -50, 150 );
    this->scene()->addItem( this->goalLeft );

    this->goalRight = new QGraphicsRectItem( 0, 0, 50, 200 );
    this->goalRight->setPen( QPen( Qt::white ) );
    this->goalRight->setBrush( Qt::white );
    this->goalRight->setPos( 1000, 150 );
    this->scene()->addItem( this->goalRight );

    // cria a bola centralizada
    this->ball = new Ball( fieldRect );
    this->ball->setPos( fieldRect.width() / 2, fieldRect.height() / 2 );
    this->scene()->addItem( ball );

    // cria os jogadores e posiciona
    this->player1 = new Player( Player::LEFT );
    this->player1->setPos( fieldRect.width() - 900, ( fieldRect.height() / 2 ) - 65 );
    this->scene()->addItem( player1 );

    this->player2 = new Player( Player::RIGHT );
    this->player2->setPos( fieldRect.width() - 135, ( fieldRect.height() / 2 ) - 65 );
    this->scene()->addItem( player2 );

    this->scoreBoard = new ScoreBoard();
    this->scoreBoard->setPos( 0, 500 );
    this->scene()->addItem( this->scoreBoard );
}

/**
 * Destrutor.
 * Fecha a porta serial e libera a memória alocada pelos objetos.
 */
Game::~Game()
{
    if ( this->port != NULL ) {
        this->port->close();
        delete this->port;
    }

    delete this->timer;
    delete this->gameTime;
    delete this->field;
    delete this->ball;
    delete this->scoreBoard;
    delete this->displayedText;
}

/**
 * Inicializa as configurações padrão para o QGraphicsView.
 *
 * Chamado no construtor para definir as opções de como o jogo deve ser
 * renderizado, como tamanho da tela, cache, antialiasing, etc.
 */
void Game::initializeConfig()
{
    this->setFrameShape( NoFrame );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->setRenderHints( QPainter::Antialiasing |
                          QPainter::TextAntialiasing |
                          QPainter::SmoothPixmapTransform |
                          QPainter::HighQualityAntialiasing |
                          QPainter::NonCosmeticDefaultPen );
    this->setDragMode( NoDrag );
    this->setCacheMode( CacheBackground );
    this->setViewportUpdateMode( MinimalViewportUpdate );
    this->setCursor( QPixmap( 1, 1 ) );
}

/**
 * Método que permite iniciar um novo jogo.
 *
 * Configura a porta serial e inicializa o contador de frames, utilizado para
 * atualizar a tela do jogo.
 *
 * Na prática, esse é o método que dá o pontapé inicial do jogo.
 */
void Game::play()
{
    this->configureSerialPort();

    // inicializa o contador de frames
    this->timer = new QTimer( this );
    this->gameTime = new QTime();

    // conecta o sinal timeout do contador com o slot do servidor
    if ( SERVER == this->gameMode ) {
        connect( this->timer, SIGNAL(timeout()), this, SLOT(playOnServer()) );
    }
    else if ( CLIENT == this->gameMode ) {
        connect( this->timer, SIGNAL(timeout()), this, SLOT(playOnClient()) );
    }
    else {
        qApp->exit( ERR_BAD_GAME_MODE );
    }

    this->timer->start( 1000 / 20 );  // 20 FPS
    this->gameTime->start();

    // captura o teclado para esperar pelas teclas de controle do jogador
    this->grabKeyboard();
    this->setMouseTracking( true );
    this->grabMouse();

    // Locutor: bola rolando, começa o jogo do Servidor X Cliente aqui no estádio do Qt ;-)
}

void Game::readyToPlay()
{
    // se o outro jogador já informou que está pronto para o jogo, começa logo
    if ( this->otherReady ) {
        this->play();
        this->removeMessage();
    }
    else {
        this->showMessage( "Aguardando outro jogador...", -1 );
    }
}

void Game::playerCollision(){
    // Colisão player1 (server)

    //Colisão frontal
    if (this->ball->getAngle() < 1.5*M_PI && this->ball->getAngle() > 0.5*M_PI){
        if ((this->ball->x()-15 <= this->player1->x()+35) &&   //Se bate na linha do jogador
            (this->ball->y()+15 >= this->player1->y()) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()-15 <= this->player1->y()+130) &&  //Se esta acima do final do jogador
            (this->ball->x()-15 > this->player1->x()+10)       //Verifica se já passou do jogador
           ){
            this->ball->setAngle((this->ball->y())-(this->player1->y() + this->player1->boundingRect().height()/2), 1);
        }
    }
    //Colisão traseira
    if (this->ball->getAngle() > 1.5*M_PI || this->ball->getAngle() < 0.5*M_PI){
        if ((this->ball->x()+15 >= this->player1->x()) &&      //Se bate na linha do jogador
            (this->ball->y()+15 >= this->player1->y()) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()-15 <= this->player1->y()+130) &&  //Se esta acima do final do jogador
            (this->ball->x()+15 < this->player1->x()+25)       //Verifica se já passou do jogador
           ){
            this->ball->setAngle(71, 1);
        }
    }
    //Colisão superior
    if ((this->ball->y()+15 >= this->player1->y()) &&      //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player1->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player1->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()+15 < this->player1->y()+25)       //Verifica se já passou do jogador
       ){
        this->ball->setAngle(72, 1);
    }
    //Colisão inferior
    if ((this->ball->y()-15 <= this->player1->y()+130) &&  //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player1->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player1->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()-15 > this->player1->y()+105)      //Verifica se já passou do jogador
       ){
        this->ball->setAngle(73, 1);

    }

    //Colisão player2 (cliente)
    //Colisão frontal
    if (this->ball->getAngle() > 1.5*M_PI || this->ball->getAngle() < 0.5*M_PI){
        if ((this->ball->x()+15 >= this->player2->x()) &&          //Se bate na linha do jogador
            (this->ball->y()-15 <= this->player2->y()+130) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()+15 >= this->player2->y()) &&          //Se esta acima do final do jogador
            (this->ball->x()+15 < this->player2->x()+25)           //Verifica se já passou do jogador
           ){
            this->ball->setAngle((this->ball->y())-(this->player2->y() + this->player2->boundingRect().height()/2)*(-1), 2);
        }
    }
    //Colisão traseira
    if (this->ball->getAngle() < 1.5*M_PI && this->ball->getAngle() > 0.5*M_PI){
        if ((this->ball->x()-15 <= this->player2->x()+35) &&      //Se bate na linha do jogador
            (this->ball->y()+15 >= this->player2->y()) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()-15 <= this->player2->y()+130) &&  //Se esta acima do final do jogador
            (this->ball->x()+15 > this->player2->x()+10)       //Verifica se já passou do jogador
           ){
            this->ball->setAngle(71, 2);
        }
    }
    //Colisão superior
    if ((this->ball->y()+15 >= this->player2->y()) &&      //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player2->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player2->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()+15 < this->player2->y()+25)       //Verifica se já passou do jogador
       ){
        this->ball->setAngle(72, 2);
    }
    //Colisão inferior
    if ((this->ball->y()-15 <= this->player2->y()+130) &&  //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player2->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player2->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()-15 > this->player2->y()+105)      //Verifica se já passou do jogador
       ){
        this->ball->setAngle(73, 2);
    }

}


void Game::setMoveUpKeyCode( Qt::Key key ){
    this->moveUpKeyCode=key;
}

void Game::setMoveDownKeyCode( Qt::Key key ){
    this->moveDownKeyCode=key;
}

/**
 * Slot utilizado para aumentar a velocidade da bola.
 * Recebe os eventos tanto do botão quanto da tecla de atalho.
 */
void Game::accelerate()
{
    this->ball->accelerate();
}

/**
 * Slot utilizado para diminuir a velocidade da bola.
 * Recebe os eventos tanto do botão quanto da tecla de atalho.
 */
void Game::deaccelerate()
{
    this->ball->deaccelerate();
}

/**
 * Define o modo de jogo.
 * @see Game::GameMode
 */
void Game::setGameMode( GameMode mode )
{
    this->gameMode = mode;
}

/**
 * Obtém o modo de jogo atual.
 * @see Game::GameMode
 * @return Se o jogo foi (ou vai ser) iniciado em modo servidor ou modo cliente.
 */
Game::GameMode Game::getGameMode() const
{
    return this->gameMode;
}

/**
 * Define o nome da porta serial.
 */
void Game::setPortName( QString port )
{
    this->portName = port;
}

/**
 * Obtém o nome da porta serial atual.
 * @return O nome da porta configurada.
 */
QString Game::getPortName() const
{
    return this->portName;
}

Qt::Key  Game::getMoveUpKeyCode (){
    return this->moveUpKeyCode;
}

Qt::Key  Game::getMoveDownKeyCode (){
    return this->moveDownKeyCode;
}

/**
 * Verifica se o jogo está ativo ou não.
 *
 * @return O método retorna false se nenhum jogo foi iniciado ou se o jogo já
 *         foi finalizado.
 */
bool Game::isPlaying() const
{
    return ( NULL != this->timer && this->timer->isActive() );
}

/**
 * Método interno utilizado para configurar a porta serial.
 * Abre a porta definida em Game::portName com as configurações padrão
 * utilizadas para o jogo.
 *
 * @note As configurações padrão são:
 *  - Baud rate         = 57.600 bauds
 *  - Bits de dados     = 8
 *  - Paridade          = nenhuma
 *  - Bits de parada    = 1
 *  - Controle de fluxo = nenhum
 *  - Tempo limite      = 200ms
 *  - Buffer            = nenhum
 */
void Game::configureSerialPort()
{
    // se a porta está aberta ela precisa ser fechada e destruída
    if ( this->port != NULL ) {
        this->port->close();
        delete this->port;
    }

    this->port = new QextSerialPort( this->portName, QextSerialPort::Polling );
    this->port->setBaudRate( BAUD57600 );
    this->port->setDataBits( DATA_8 );
    this->port->setParity( PAR_NONE );
    this->port->setStopBits( STOP_1 );
    this->port->setFlowControl( FLOW_OFF );
    this->port->setTimeout( 200 );
    this->port->open( QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate );
}

void Game::keyPressEvent( QKeyEvent * event )
{
    if ( !this->isPlaying() ) {
        return;
    }

    if ( this->moveUpKeyCode == event->key() ) {
        event->accept();
        if (gameMode == SERVER){
            player1->toup();
        }
        else {
            player2->toup();
        }
    }
    else if ( this->moveDownKeyCode == event->key() ) {
        event->accept();
        if (gameMode == SERVER){
            player1->todown();
        }
        else {
            player2->todown();
        }
    }
    else if ( Qt::Key_Escape == event->key() ) {
        this->releaseMouse();
    }
}

bool Game::eventFilter( QObject * obj, QEvent * event )
{
    if ( QEvent::MouseMove == event->type() ) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(event);

        if ( mouseEvent->pos().y() < this->sceneRect().center().y() ) {
            if ( SERVER == gameMode ) {
                player1->toup();
            }
            else {
                player2->toup();
            }
        }
        else if ( mouseEvent->pos().y() > this->sceneRect().center().y() ) {
            if ( SERVER == gameMode ) {
                player1->todown();
            }
            else {
                player2->todown();
            }
        }

        // retorna o mouse para o centro da tela (necessário para nunca sair dos limites)
        QCursor::setPos( this->mapToGlobal( this->sceneRect().center().toPoint() ) );

        return true;
    }

    return false;
}

/**
 * Slot privado que controla o jogo no lado do servidor.
 *
 * Esse é o método responsável por calcular todos os movimentos do jogo e
 * depois enviar essas informações para o cliente através da comunicação serial.
 *
 * @todo Receber dados do cliente e calcular movimentos do jogador.
 *
 * @see Game::play
 * @see Game::configureSerialPort
 */
void Game::playOnServer()
{
    // jogo só pode ser jogado com a conexão estabelecida
    if ( this->port == NULL || !this->port->isOpen() ) {
        return;
    }

    if ( this->port->bytesAvailable() > 0 )
        QByteArray read = this->port->read( sizeof(ClientInfo) );

    // realiza os cálculos no servidor
    this->scene()->advance();
    this->ball->rotate();

    // verificações
    this->verifyGoal();
    this->playerCollision();

    // envia os novos dados para o cliente
    QByteArray data;
    GameControl info;
    info.ballX       = this->ball->x();
    info.ballY       = this->ball->y();
    info.playerLeft  = 0;    // TODO
    info.playerRight = this->player1->y();
    info.scoreLeft   = 0;    // TODO
    info.scoreRight  = 0;    // TODO
    info.gameSeconds = this->gameTime->elapsed() / 1000;
    info.ballReverse = this->ball->getReversed();

    data.setRawData( (char*) &info, sizeof(GameControl) );
    this->port->write(data);

    // atualiza o placar atual
    this->scoreBoard->setTime( info.gameSeconds );
}

/**
 * Slot privado que controla o jogo no lado do cliente.
 *
 * Esse é o método responsável por receber as informações do servidor através
 * da comunicação serial e aplicá-las ao jogo.
 *
 * @todo Enviar dados para o servidor.
 *
 * @see Game::play
 * @see Game::configureSerialPort
 */
void Game::playOnClient()
{
    // jogo só pode ser jogado com a conexão estabelecida
    if ( this->port == NULL || !this->port->isOpen() ) {
        return;
    }

    QByteArray data;
    ClientInfo client;
    client.playerMovement = 0;   // movimento do jogador
    data.setRawData( (char*) &client, sizeof(ClientInfo) );
    this->port->write( data );

    QByteArray read = this->port->read( sizeof(GameControl) );
    GameControl * info = (GameControl*) read.data();

    this->ball->setX( info->ballX );
    this->ball->setY( info->ballY );
    this->ball->rotate( info->ballReverse );
    this->player1->setY( info->playerLeft );
    this->scoreBoard->setTime( info->gameSeconds );
}

/**
 * Método utilizado para exibir uma mensagem sobre o jogo.
 *
 * A mensagem será exibida no centro da tela, sobre os outros objetos do jogo.
 * Apenas uma mensagem pode ser exibida por vez. Se já houver uma em exibição,
 * a mensagem não será exibida.
 *
 * @param msg A mensagem a ser exibida. Pode conter HTML.
 * @param time O tempo (em milissegundos) para a mensagem ser exibida. Passe um
 *        valor negativo para a mensagem não ser removida automaticamente (chame
 *        o método Game::removeMessage para removê-la). O valor padrão é 5000
 *        (5 segundos).
 * @return Retorna true se a mensagem pode ser exibida. Senão, retorna false.
 * @note A mensagem não é bloqueante (ou seja, a exibição dela não faz com que
 *       o jogo fique parado).
 */
bool Game::showMessage( QString msg, int time )
{
    if ( this->displayedText != NULL ) {
        return false;
    }

    // efeito de sombra para deixar o texto mais legível
    this->displayedTextEffect = new QGraphicsDropShadowEffect();
    this->displayedTextEffect->setColor( Qt::black );
    this->displayedTextEffect->setOffset( 10 );
    this->displayedTextEffect->setBlurRadius( 25 );

    // cria o texto centralizado
    this->displayedText = new QGraphicsTextItem();
    this->displayedText->setTextWidth( this->field->rect().width() );
    this->displayedText->setDefaultTextColor( Qt::yellow );
    this->displayedText->setFont( QFont( "sans-serif", 50, QFont::Bold ) );
    this->displayedText->setHtml( "<center style='line-height: 350px'>" + msg + "</center>" );
    this->displayedText->setGraphicsEffect( this->displayedTextEffect );
    this->scene()->addItem( this->displayedText );

    if ( time > 0 ) {
        QTimer::singleShot( time, this, SLOT(removeMessage()) );
    }

    return true;
}

/**
 * Remover a mensagem adicionada com Game::showMessage.
 *
 * Caso a mensagem tenha sido configurada para não ser removida automaticamente,
 * esse método deverá ser chamado para removê-la.
 *
 * Se a mensagem foi configurada para ser removida automaticamente após um certo
 * tempo, esse método é chamado por QTimer::singleShot para remover a mensagem
 * da tela.
 */
void Game::removeMessage()
{
    if ( this->displayedTextEffect != NULL ) {
        delete this->displayedTextEffect;
        this->displayedTextEffect = NULL;
    }
    if ( this->displayedText != NULL ) {
        this->scene()->removeItem( this->displayedText );
        delete this->displayedText;
        this->displayedText = NULL;
    }
}

/**
 * Evento de redimensionamento da área do jogo.
 *
 * Responsável por mater todo o jogo visível, o máximo possível, sem alterar
 * a proporção do tamanho original.
 */
void Game::resizeEvent( QResizeEvent * event)
{
    this->fitInView( this->sceneRect(), Qt::KeepAspectRatio );
}

void Game::verifyGoal()
{
    int ballRadius = this->ball->boundingRect().width() / 2,
        ballX = this->ball->x(),
        ballY = this->ball->y(),
        limitTop = this->goalLeft->y(),
        limitBottom = this->goalLeft->y() + this->goalLeft->rect().height();

    // na esquerda
    if ( ballX - ballRadius == 0 && ballY - ballRadius >= limitTop && ballY + ballRadius <= limitBottom ) {
        this->showMessage( "GOOL!", 3000 );
        this->timer->stop();
        QTimer::singleShot( 3000, this->timer, SLOT(start()) );
    }
    // na direita
    else if ( ballX + ballRadius == this->field->rect().width() && ballY - ballRadius >= limitTop && ballY + ballRadius <= limitBottom ) {
        this->showMessage( "GOOL!", 3000 );
        this->timer->stop();
        QTimer::singleShot( 3000, this->timer, SLOT(start()) );
    }
}
