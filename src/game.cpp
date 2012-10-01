#include <QGraphicsView>
#include <QTimer>

#include "ball.h"
#include "game.h"
#include "gameoptions.h"
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
    this->port = NULL;
    this->timer = NULL;
    this->initializeConfig();

    GameOptions * op = new GameOptions( this );
    op->show();

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
    connect( this->timer, SIGNAL(timeout()), this->scene(), SLOT(advance()) );
    this->timer->start( 1000 / 20 );  // 20 FPS

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
 */
QString Game::getPortName() const
{
    return this->portName;
}

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
 *  - Baud rate         = 38400
 *  - Bits de dados     = 7
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

    this->port = new QextSerialPort( this->portName, QextSerialPort::EventDriven );
    this->port->setBaudRate( BAUD38400 );
    this->port->setDataBits( DATA_7 );
    this->port->setParity( PAR_NONE );
    this->port->setStopBits( STOP_1 );
    this->port->setFlowControl( FLOW_OFF );
    this->port->setTimeout( 200 );
    this->port->open( QIODevice::ReadWrite | QIODevice::Unbuffered );
}
