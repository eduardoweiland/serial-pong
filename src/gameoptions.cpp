#include <QDialog>
#include <QKeyEvent>
#include <QDebug>

#include "gameoptions.h"
#include "ui_gameoptions.h"
#include "globals.h"
#include "game.h"

/**
 * Construtor.
 * Cria uma nova janela de opções do jogo.
 */
GameOptions::GameOptions( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::GameOptions )
{
    this->ui->setupUi( this );

    this->moveUpKeyCode = this->reservedKey;
    this->moveDownKeyCode = this->reservedKey;
    this->grabbingKey = -1;

    // oculta o modo avançado
    this->ui->groupAdvanced->hide();

    // porta serial padrão
    this->ui->editSerialPort->setText( SERIALPORT );

    // conecta signals e slots (eventos)
    connect( this->ui->btnMoveUp,   SIGNAL(toggled(bool)), this, SLOT(btnMoveUpToggled(bool)) );
    connect( this->ui->btnMoveDown, SIGNAL(toggled(bool)), this, SLOT(btnMoveDownToggled(bool)) );
    connect( this->ui->btnExit,     SIGNAL(clicked()),     this, SLOT(reject()) );
    connect( this->ui->btnPlay,     SIGNAL(clicked()),     this, SLOT(accept()) );
    connect( this->ui->btnAdvanced, SIGNAL(toggled(bool)), this->ui->groupAdvanced, SLOT(setVisible(bool)) );

    // validações
    connect( this->ui->rdbServerMode,  SIGNAL(toggled(bool)),        this, SLOT(validateConfig()) );
    connect( this->ui->rdbClientMode,  SIGNAL(toggled(bool)),        this, SLOT(validateConfig()) );
    connect( this->ui->btnMoveUp,      SIGNAL(toggled(bool)),        this, SLOT(validateConfig()) );
    connect( this->ui->btnMoveDown,    SIGNAL(toggled(bool)),        this, SLOT(validateConfig()) );
    connect( this->ui->editSerialPort, SIGNAL(textChanged(QString)), this, SLOT(validateConfig()) );
}

/**
 * Destrutor da janela.
 * Libera a memória alocada.
 */
GameOptions::~GameOptions()
{
    delete this->ui;
}

/**
 * Evento de clique no botão "Mover o jogador para cima".
 * Entra em modo de "escuta de teclas" para permitir que o usuário escolha.
 *
 * @param pressed Se o botão foi pressionado ou despressionado.
 */
void GameOptions::btnMoveUpToggled( bool pressed )
{
    if ( pressed ) {
        this->ui->btnMoveDown->setChecked( false );
        this->grabKeyboard();
        this->grabbingKey = BTN_MOVEUP;
    } else {
        this->releaseKeyboard();
        this->grabbingKey = -1;
    }
}

/**
 * Evento de clique no botão "Mover o jogador para baixo".
 * Entra em modo de "escuta de teclas" para permitir que o usuário escolha.
 *
 * @param pressed Se o botão foi pressionado ou despressionado.
 */
void GameOptions::btnMoveDownToggled( bool pressed )
{
    if ( pressed ) {
        this->ui->btnMoveUp->setChecked( false );
        this->grabKeyboard();
        this->grabbingKey = BTN_MOVEDOWN;
    } else {
        this->releaseKeyboard();
        this->grabbingKey = -1;
    }
}

/**
 * Evento de pressionamento de uma tecla.
 * Utilizado quando o jogador clica em um dos botões para escolher a tecla de
 * movimentação.
 *
 * @param event O evento de pressionamento da tecla.
 */
void GameOptions::keyPressEvent( QKeyEvent * event )
{
    // não aceita teclas especiais (Ctrl, Shift, ...)
    if ( Qt::NoModifier != event->modifiers() ||
       ( this->grabbingKey != BTN_MOVEUP && this->grabbingKey != BTN_MOVEDOWN ) ) {
        event->ignore();
        return;
    }

    event->accept();

    if ( BTN_MOVEDOWN == this->grabbingKey ) {
        this->ui->btnMoveDown->setChecked( false );
        this->ui->btnMoveDown->setText( this->getKeyString( event->key() ) );
        this->moveDownKeyCode = (Qt::Key) event->key();
    }
    else if ( BTN_MOVEUP == this->grabbingKey ) {
        this->ui->btnMoveUp->setChecked( false );
        this->ui->btnMoveUp->setText( this->getKeyString( event->key() ) );
        this->moveUpKeyCode = (Qt::Key) event->key();
    }

    this->grabbingKey = -1;
    this->releaseKeyboard();
    this->validateConfig();
}

/**
 * Método auxiliar que retorna a representação gráfica de uma tecla.
 * Utilizado para exibir qual tecla foi pressionada pelo usuário.
 *
 * @param code O código da tecla pressionada.
 * @return Uma string que representa a tecla.
 * @note Para teclas que não são permitidas o método retorna tecla desconhecida.
 */
QString GameOptions::getKeyString( int code ) const
{
    if ( code >= Qt::Key_Exclam && code <= Qt::Key_AsciiTilde ) {
        return QString( QChar( code ) );
    }

    QString str;

    switch ( code ) {
        case Qt::Key_Down:      str = "Seta para baixo";                    break;
        case Qt::Key_Up:        str = "Seta para cima";                     break;
        case Qt::Key_Left:      str = "Seta para esquerda";                 break;
        case Qt::Key_Right:     str = "Seta para direita";                  break;
        case Qt::Key_Space:     str = QString::fromUtf8("Barra de espaço"); break;
        case Qt::Key_Enter:     str = "Enter";                              break;
        case Qt::Key_Backspace: str = "Backspace";                          break;
        case Qt::Key_Home:      str = "Home";                               break;
        case Qt::Key_End:       str = "End";                                break;
        case Qt::Key_Insert:    str = "Insert";                             break;
        case Qt::Key_Delete:    str = "Delete";                             break;
        case Qt::Key_PageUp:    str = "Page Up";                            break;
        case Qt::Key_PageDown:  str = "Page Down";                          break;
        case Qt::Key_Escape:    str = "Esc";                                break;
        case Qt::Key_F1:        str = "F1";                                 break;
        case Qt::Key_F2:        str = "F2";                                 break;
        case Qt::Key_F3:        str = "F3";                                 break;
        case Qt::Key_F4:        str = "F4";                                 break;
        case Qt::Key_F5:        str = "F5";                                 break;
        case Qt::Key_F6:        str = "F6";                                 break;
        case Qt::Key_F7:        str = "F7";                                 break;
        case Qt::Key_F8:        str = "F8";                                 break;
        case Qt::Key_F9:        str = "F9";                                 break;
        case Qt::Key_F10:       str = "F10";                                break;
        case Qt::Key_F11:       str = "F11";                                break;
        case Qt::Key_F12:       str = "F12";                                break;
        default:                str = "<tecla desconhecida>";               break;
        // TODO: teclas, teclas, e mais teclas aqui...
    }

    return str;
}

/**
 * Método utilizado para validar as configurações e habilitar ou desabilitar o
 * botão "Play" conforme a condição atual.
 *
 * O método é um slot para permitir conectá-lo diretamente aos signals enviados
 * pelos itens da interface.
 */
void GameOptions::validateConfig()
{
    bool valid = true;
    valid = valid && this->getGameMode() != Game::UNKNOWN;
    valid = valid && !this->getSerialPort().isEmpty();
    valid = valid && this->getMoveUpKey() != this->reservedKey;
    valid = valid && this->getMoveDownKey() != this->reservedKey;
    valid = valid && this->getMoveUpKey() != this->getMoveDownKey();

    this->ui->btnPlay->setDisabled(!valid);
}

QString GameOptions::getSerialPort() const
{
    return this->ui->editSerialPort->text();
}

void GameOptions::setSerialPort( QString portName )
{
    this->ui->editSerialPort->setText( portName );
}

Qt::Key GameOptions::getMoveUpKey() const
{
    return this->moveUpKeyCode;
}

void GameOptions::setMoveUpKey( Qt::Key keyCode )
{
    this->moveUpKeyCode = keyCode;
    this->ui->btnMoveUp->setText( this->getKeyString( keyCode ) );
}

Qt::Key GameOptions::getMoveDownKey() const
{
    return this->moveDownKeyCode;
}

void GameOptions::setMoveDownKey( Qt::Key keyCode )
{
    this->moveDownKeyCode = keyCode;
    this->ui->btnMoveDown->setText( this->getKeyString( keyCode ) );
}

bool GameOptions::getEnableMouse() const
{
    return this->ui->chbEnableMouse->isChecked();
}

void GameOptions::setEnableMouse( bool enabled )
{
    this->ui->chbEnableMouse->setChecked( enabled );
}

Game::GameMode GameOptions::getGameMode() const
{
    if ( this->ui->rdbServerMode->isChecked() ) {
        return Game::SERVER;
    }
    else if ( this->ui->rdbClientMode->isChecked() ) {
        return Game::CLIENT;
    }

    return Game::UNKNOWN;
}

void GameOptions::setGameMode( Game::GameMode mode )
{
    this->ui->rdbServerMode->setChecked( Game::SERVER == mode );
    this->ui->rdbClientMode->setChecked( Game::CLIENT == mode );
}

QString GameOptions::getPlayerName() const
{
    return this->ui->editPlayerName->text();
}

void GameOptions::setPlayerName( QString name )
{
    this->ui->editPlayerName->setText( name );
}
