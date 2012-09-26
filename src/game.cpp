#include "ball.h"
#include "game.h"
#include "gameoptions.h"
#include "qextserialport.h"

/**
 * Construtor padrão.
 * Cria um novo jogo, abrindo primeiro a tela de configuração.
 */
Game::Game()
{
    GameOptions op;
    op.show();

    // classe não herda nenhuma classe do Qt então isso não funciona:
    //QObject::connect( &op, SIGNAL(accepted()), this, SLOT(play()) );
}

/**
 * Destrutor.
 * Fecha a porta serial e libera a memória alocada pelos objetos.
 */
Game::~Game()
{
    this->port->close();
    delete this->port;
}

/**
 * Método que permite iniciar um novo jogo.
 * @todo Dá pra ver que ainda precisa implementar...
 */
void Game::play()
{
    if ( !this->port->isOpen() ) {
        this->configureSerialPort();
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
    this->port = new QextSerialPort( this->portName, QextSerialPort::EventDriven );
    this->port->setBaudRate( BAUD38400 );
    this->port->setDataBits( DATA_7 );
    this->port->setParity( PAR_NONE );
    this->port->setStopBits( STOP_1 );
    this->port->setFlowControl( FLOW_OFF );
    this->port->setTimeout( 200 );
    this->port->open( QIODevice::ReadWrite | QIODevice::Unbuffered );
}
