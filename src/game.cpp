#include <QGraphicsView>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

#include "ball.h"
#include "game.h"
#include "gameoptions.h"
#include "globals.h"
#include "qextserialport.h"

/**
 * Construtor padrão.
 * Cria um novo jogo, abrindo primeiro a tela de configuração.
 *
 * A classe herda de QGraphicsView e representa toda a área do jogo.
 *
 * @see http://qt-project.org/doc/qt-4.8/qgraphicsview.html#QGraphicsView
 */
Game::Game( QWidget * parent ) :
    QGraphicsView( new QGraphicsScene( 0, 0, 1000, 500 ), parent )
{
    this->port     = NULL;
    this->portName = SERIALPORT;
    this->timer    = NULL;
    this->gameTime = NULL;
    this->gameMode = CLIENT;
    this->initializeConfig();

    GameOptions * op = new GameOptions( this );
    op->show();

    // TODO: trazer as configurações do jogo
    connect( op, SIGNAL(accepted()), this, SLOT(play()) );

    // cria a cena para conter todos os itens do jogo.
    this->scene()->setBackgroundBrush( Qt::black );

    // o campo do jogo (a bola não deve sair dele)
    this->field = new QGraphicsRectItem( this->sceneRect() );
    this->field->setBrush( Qt::darkGreen );
    this->scene()->addItem( this->field );

    // cria a bola centralizada
    this->ball = new Ball();
    this->ball->setPos( this->scene()->width() / 2, this->scene()->height() / 2 );
    this->scene()->addItem( ball );
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
    delete this->field;
    delete this->ball;
}

/**
 * Inicializa as configurações padrão para o QGraphicsView.
 * Chamado no construtor para definir as opções de como o jogo deve ser
 * renderizado, como tamanho da tela, cache, antialiasing, etc.
 */
void Game::initializeConfig()
{
    this->setMinimumSize( this->scene()->width(), this->scene()->height() );
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
}

/**
 * Método que permite iniciar um novo jogo.
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
        exit( ERR_BAD_MODE );
    }

    this->timer->start( 1000 / 20 );  // 20 FPS
    this->gameTime->start();          // tempo de jogo

    // Locutor: bola rolando, começa o jogo do Servidor X Cliente aqui no estádio do Qt ;-)
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

/**
 * Verifica se o jogo está ativo ou não.
 *
 * @return O método retorna false se nenhum jogo foi iniciado ou se o jogo já
 *         foi finalizado.
 *
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
 *  - Baud rate         = 57600
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
    this->port->open( QIODevice::ReadWrite | QIODevice::Unbuffered );
}


void Game::playOnServer()
{
    // jogo só pode ser jogado com a conexão estabelecida
    if ( this->port == NULL || !this->port->isOpen() ) {
        return;
    }

    // realiza os cálculos no servidor
    this->scene()->advance();

    // envia os novos dados para o cliente
    QByteArray data;
    GameControl info = {};
    info.ballX       = this->ball->x();
    info.ballY       = this->ball->y();
    info.playerLeft  = 0;    // TODO
    info.playerRight = 0;    // TODO
    info.scoreLeft   = 0;    // TODO
    info.scoreRight  = 0;    // TODO
    info.gameSeconds = this->gameTime->elapsed() / 1000;

    data.setRawData( (char*) &info, sizeof(GameControl) );
    this->port->write(data);
}

void Game::playOnClient()
{
    // jogo só pode ser jogado com a conexão estabelecida
    if ( this->port == NULL || !this->port->isOpen() || this->port->bytesAvailable() < sizeof(GameControl) ) {
        // se o jogo já estava ocorrendo, então a conexão foi perdida
        if ( this->isPlaying() ) {
           // QMessageBox::critical( this, QString::fromUtf8( "Conexão perdida" ),
           //     QString::fromUtf8( "A conexão com o servidor foi perdida, tente novamente mais tarde" ) );
           // this->timer->stop();
        }
        return;
    }

    QByteArray read = this->port->read( sizeof(GameControl) );
    GameControl * info = (GameControl*) read.data();

    this->ball->setX( info->ballX );
    this->ball->setY( info->ballY );

    qDebug() << info->gameSeconds;
}
