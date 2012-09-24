#include "ball.h"
#include "game.h"
#include "gameoptions.h"
#include "qextserialport.h"

Game::Game()
{
    GameOptions op;
    op.show();

 //   QObject::connect( &op, SIGNAL(accepted()), this, SLOT(play()) );
}

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
 * Método interno utilizado para configurar a porta serial.
 * Abre a porta definida em {@link ::portName} com as configurações padrão
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
