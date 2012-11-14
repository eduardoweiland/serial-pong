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
    // configurações padrão do jogo
    this->portName        = SERIALPORT;             // porta serial COM1 (/dev/ttyS0)
    this->gameMode        = UNKNOWN;                // modo desconhecido
    this->moveUpKeyCode   = Qt::Key_nobreakspace;   // tecla desconhecida
    this->moveDownKeyCode = Qt::Key_nobreakspace;   // tecla desconhecida
    this->moveWithMouse   = false;                  // movimento com o mouse desabilitado

    // inicializa os controles do jogo
    this->port                = NULL;   // conexão serial
    this->timer               = NULL;   // timer para atualizar a tela
    this->gameTime            = NULL;   // tempo de jogo
    this->displayedText       = NULL;   // mensagens exibidas sobre o jogo
    this->displayedTextEffect = NULL;   // efeito de sombra na mensagem
    this->player1score        = 0;      // número de gols do jogador 1
    this->player2score        = 0;      // número de gols do jogador 2
    this->otherReady          = false;  // adversário não está pronto
    this->paused              = true;   // jogo pausado (esperando adversário)
    this->localPlayerName     = "";     // nome do jogador local
    this->remotePlayerName    = "";     // nome do jogador remoto/adversário

    // inicializa opções de renderização do jogo
    this->initializeConfig();
    qApp->installEventFilter( this );

    /// @todo Talvez uma torcida ao redor do campo de jogo?!
    this->scene()->setBackgroundBrush( Qt::black );

    // o campo do jogo (a bola não deve sair dele)
    QRectF fieldRect = QRectF( 0, 0, 1000, 500 );
    this->field = new QGraphicsRectItem( fieldRect );
    this->field->setBrush( QPixmap ( ":/background.png" ) );
    this->scene()->addItem( this->field );

    // goleira do lado esquerdo do campo
    this->goalLeft = new QGraphicsRectItem( 0, 0, 50, 200 );
    this->goalLeft->setPen( QPen( Qt::white ) );
    this->goalLeft->setBrush( Qt::white );
    this->goalLeft->setPos( -50, 150 );
    this->scene()->addItem( this->goalLeft );

    // goleira do lado direito do campo
    this->goalRight = new QGraphicsRectItem( 0, 0, 50, 200 );
    this->goalRight->setPen( QPen( Qt::white ) );
    this->goalRight->setBrush( Qt::white );
    this->goalRight->setPos( 1000, 150 );
    this->scene()->addItem( this->goalRight );

    // cria a bola centralizada no campo
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

    // cria o placar do jogo
    this->scoreBoard = new ScoreBoard();
    this->scoreBoard->setPos( 0, fieldRect.height() );
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
    delete this->scoreBoard;

    delete this->field;
    delete this->goalLeft;
    delete this->goalRight;
    delete this->ball;
    delete this->player1;
    delete this->player2;

    if ( NULL != this->displayedTextEffect ) delete this->displayedTextEffect;
    if ( NULL != this->displayedText )       delete this->displayedText;
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
//    this->configureSerialPort();

    // inicializa o contador de frames
    this->timer = new QTimer( this );
    this->gameTime = new QTime();

    // conecta o sinal timeout do contador com o slot do servidor
    if ( SERVER == this->gameMode ) {
        connect( this->timer, SIGNAL(timeout()), this, SLOT(playOnServer()) );
        this->scoreBoard->setLeftPlayerName( this->localPlayerName );
        this->scoreBoard->setRightPlayerName( this->remotePlayerName );
    }
    else if ( CLIENT == this->gameMode ) {
        connect( this->timer, SIGNAL(timeout()), this, SLOT(playOnClient()) );
        this->scoreBoard->setLeftPlayerName( this->remotePlayerName );
        this->scoreBoard->setRightPlayerName( this->localPlayerName );
    }
    else {
        qApp->exit( ERR_BAD_GAME_MODE );
    }

    this->timer->start( 1000 / 20 );  // 20 FPS
    this->gameTime->start();
    this->paused = false;

    // captura o teclado para esperar pelas teclas de controle do jogador
    this->grabKeyboard();

    if ( this->moveWithMouse ) {
        this->setMouseTracking( true );
        this->grabMouse();
    }
}

void Game::readyToPlay()
{

#ifdef SP_BUILD_DEBUG
    this->play();
    return;
#endif

    if ( NULL == this->port || !this->port->isOpen() ) {
        this->configureSerialPort();
    }

    // se o outro jogador já informou que está pronto para o jogo, começa logo
    if ( this->otherReady ) {
        this->removeMessage();
        this->play();
    }
    else {
        this->showMessage( "Aguardando outro jogador...", -1 );
        this->timer = new QTimer( this );
        this->timer->start( 50 );
        connect( this->timer, SIGNAL(timeout()), this, SLOT(waitPlayer()) );
    }
}

void Game::waitPlayer()
{
    // informa ao outro jogador que está pronto para jogar
    Greetings info;
    info.ready = true;
    strcpy( info.name, this->localPlayerName.toAscii().data() );
    this->port->write( (char*) &info, sizeof(Greetings) );

    // verifica se o outro jogador enviou informações
    if ( this->port->bytesAvailable() >= sizeof(Greetings) ) {
        Greetings remoteInfo;
        this->port->read( (char*) &remoteInfo, sizeof(Greetings) );

        // não precisamos mais desse evento
        this->timer->stop();
        disconnect( this->timer, SIGNAL(timeout()), this, SLOT(waitPlayer()) );
        delete this->timer;

        this->otherReady = remoteInfo.ready;
        this->remotePlayerName = remoteInfo.name;
        this->readyToPlay();
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
    else if (this->ball->getAngle() > 1.5*M_PI || this->ball->getAngle() < 0.5*M_PI){
        if ((this->ball->x()+15 >= this->player1->x()) &&      //Se bate na linha do jogador
            (this->ball->y()+15 >= this->player1->y()) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()-15 <= this->player1->y()+130) &&  //Se esta acima do final do jogador
            (this->ball->x()+15 < this->player1->x()+25)       //Verifica se já passou do jogador
           ){
            this->ball->setAngle(71, 1);
        }
    }
    //Colisão superior
    else if ((this->ball->y()+15 >= this->player1->y()) &&      //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player1->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player1->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()+15 < this->player1->y()+25)       //Verifica se já passou do jogador
       ){
        this->ball->setAngle(72, 1);
    }
    //Colisão inferior
    else if ((this->ball->y()-15 <= this->player1->y()+130) &&  //Se bate na linha do jogador
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
            this->ball->setAngle((this->ball->y())-(this->player2->y() + this->player2->boundingRect().height()/2), 2);
        }
    }
    //Colisão traseira
    else if (this->ball->getAngle() < 1.5*M_PI && this->ball->getAngle() > 0.5*M_PI){
        if ((this->ball->x()-15 <= this->player2->x()+35) &&      //Se bate na linha do jogador
            (this->ball->y()+15 >= this->player2->y()) &&      //Se esta abaixo do inicio do jogador
            (this->ball->y()-15 <= this->player2->y()+130) &&  //Se esta acima do final do jogador
            (this->ball->x()+15 > this->player2->x()+10)       //Verifica se já passou do jogador
           ){
            this->ball->setAngle(71, 2);
        }
    }
    //Colisão superior
    else if ((this->ball->y()+15 >= this->player2->y()) &&      //Se bate na linha do jogador
        (this->ball->x()+15 >= this->player2->x()) &&      //Se esta no inicio do jogador
        (this->ball->x()-15 <= this->player2->x()+35) &&   //Se esta no final do jogador
        (this->ball->y()+15 < this->player2->y()+25)       //Verifica se já passou do jogador
       ){
        this->ball->setAngle(72, 2);
    }
    //Colisão inferior
    else if ((this->ball->y()-15 <= this->player2->y()+130) &&  //Se bate na linha do jogador
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
    if ( this->isPlaying() ) {
        this->ball->accelerate();
    }
}

/**
 * Slot utilizado para diminuir a velocidade da bola.
 * Recebe os eventos tanto do botão quanto da tecla de atalho.
 */
void Game::deaccelerate()
{
    if ( this->isPlaying() ) {
        this->ball->deaccelerate();
    }
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

Qt::Key  Game::getMoveUpKeyCode () const
{
    return this->moveUpKeyCode;
}

Qt::Key  Game::getMoveDownKeyCode () const
{
    return this->moveDownKeyCode;
}

void Game::setMoveWithMouse( bool move )
{
    this->moveWithMouse = move;
}

bool Game::getMoveWithMouse() const
{
    return this->moveWithMouse;
}

/**
 * Verifica se o jogo está ativo ou não.
 *
 * Jogo ativo significa que a bola está rolando e ambos os jogadores podem se
 * mover.
 *
 * Se nenhum jogo foi iniciado, ou se o jogo já terminou, ou se algum jogador
 * está aguardando o adversário configurar suas opções, ou ainda em caso de gol,
 * o jogo é considerado inativo e o método retorna false.
 *
 * @return Caso o jogo seja considerado ativo retorna true, senão retorna false.
 */
bool Game::isPlaying() const
{
    return ( NULL != this->timer && this->timer->isActive() && !this->paused );
}

/**
 * Método interno utilizado para configurar a porta serial.
 *
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
    // se a porta está aberta ela precisa ser fechada
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
    if ( !this->isPlaying() ) {
        return false;
    }

    if ( this->moveWithMouse && QEvent::MouseMove == event->type() ) {
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
#ifndef SP_BUILD_DEBUG
    // jogo só pode ser jogado com a conexão estabelecida
    if ( this->port == NULL || !this->port->isOpen() ) {
        return;
    }

    // lê as informações enviadas pelo cliente
    if ( this->port->bytesAvailable() > 0 ) {
        QByteArray read = this->port->read( sizeof(ClientInfo) );
        ClientInfo * client;
        client = (ClientInfo*) read.data();

        qDebug() << client->playerPos << ":" << client->velocity;
    }

    // realiza os cálculos no servidor
    if ( !this->paused ) {
#endif
        this->scene()->advance();
        this->ball->rotate();

        // verificações
        this->verifyGoal();
        this->playerCollision();

#ifndef SP_BUILD_DEBUG
    }
#endif

#ifdef SP_BUILD_DEBUG
    return;
#endif

    // envia os novos dados para o cliente
    QByteArray data;
    GameControl info;
    info.ballX        = this->ball->x();
    info.ballY        = this->ball->y();
    info.playerLeft   = this->player1->y();
    info.scoreLeft    = this->player1score;
    info.scoreRight   = this->player2score;
    info.gameSeconds  = this->gameTime->elapsed() / 1000;
    info.ballRotation = this->ball->rotation();
    info.paused       = this->paused;

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
    client.playerPos = this->player2->y();
    client.velocity  = 6;   // TODO:

    data.setRawData( (char*) &client, sizeof(ClientInfo) );
    this->port->write( data );

    QByteArray read = this->port->read( sizeof(GameControl) );
    GameControl * info = (GameControl*) read.data();

    // bola
    this->ball->setX( info->ballX );
    this->ball->setY( info->ballY );
    this->ball->setRotation( info->ballRotation );

    // jogadores
    this->player1->setY( info->playerLeft );

    // atualiza o placar
    this->scoreBoard->setTime( info->gameSeconds );
    this->scoreBoard->setLeftScore( info->scoreLeft );
    this->scoreBoard->setRightScore( info->scoreRight );

    // controle
    this->paused = info->paused;
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
    this->displayedText->setFont( QFont( "sans-serif", 40, QFont::Bold ) );
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

/**
 * Verifica se ocorreu gol ou não.
 *
 * Esse método utiliza a posição atual da bola e o posicionamento das goleiras
 * definido no construtor para verificar se a posição em que a bola bateu é
 * considerado gol ou não.
 *
 * As duas goleiras são verificadas separadamente, e caso ocorra o gol o placar
 * __já é incrementado__.
 *
 * @return Retorna um valor bool que indica se houve ou não um gol (em qualquer
 * lado do campo).
 */
bool Game::verifyGoal()
{
    int ballRadius = this->ball->boundingRect().width() / 2,
        ballX = this->ball->x(),
        ballY = this->ball->y(),
        limitTop = this->goalLeft->y(),
        limitBottom = limitTop + this->goalLeft->rect().height();
    bool goal = false;

    // limites superior e inferior das goleiras
    if ( ballY - ballRadius >= limitTop && ballY + ballRadius <= limitBottom ) {
        // na esquerda
        if ( ballX - ballRadius <= this->field->rect().left() ) {
            this->player2score++;
            this->scoreBoard->setRightScore( this->player2score );
            goal = true;
        }
        // na direita
        else if ( ballX + ballRadius >= this->field->rect().width() ) {
            this->player1score++;
            this->scoreBoard->setLeftScore( this->player1score );
            goal = true;
        }

        if ( goal ) {
            this->showMessage( "GOOL!", 3000 );
            this->pauseGame();
            QTimer::singleShot( 3000, this, SLOT(continueGame()) );
            this->ball->resetAngle();
        }
    }

    return goal;
}

void Game::setLocalPlayerName( QString name )
{
    this->localPlayerName = name.isEmpty() ? this->localPlayerName : name;
}

void Game::setRemotePlayerName( QString name )
{
    this->remotePlayerName = name.isEmpty() ? this->remotePlayerName : name;
}

QString Game::getLocalPlayerName() const
{
    return this->localPlayerName;
}

QString Game::getRemotePlayerName() const
{
    return this->remotePlayerName;
}

void Game::pauseGame()
{
    this->paused = true;
}

void Game::continueGame( bool center )
{
    if ( center ) {
        this->ball->setPos( this->field->rect().width() / 2, this->field->rect().height() / 2 );
    }

    this->paused = false;
}
