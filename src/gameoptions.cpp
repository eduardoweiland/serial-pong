#include "gameoptions.h"
#include "ui_gameoptions.h"
#include "globals.h"

#include <QKeyEvent>

/**
 * Construtor.
 * Cria uma nova janela de opções do jogo.
 *
 * @note O jogo só deve ser iniciado depois que os dois jogadores configurarem
 *       as opções nessa janela.
 */
GameOptions::GameOptions( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::GameOptions )
{
    ui->setupUi(this);

    ui->groupAdvanced->hide();

    // porta serial padrão
    ui->editSerialPort->setText( SERIALPORT );

    // conecta signals e slots (eventos)
    connect( ui->btnMoveUp,   SIGNAL(toggled(bool)), this, SLOT(getMoveUpKey(bool)) );
    connect( ui->btnMoveDown, SIGNAL(toggled(bool)), this, SLOT(getMoveDownKey(bool)) );
    connect( ui->btnExit,     SIGNAL(clicked()),     this, SLOT(close()) );
    connect( ui->btnPlay,     SIGNAL(clicked()),     this, SLOT(accept()) );
    connect( ui->btnAdvanced, SIGNAL(toggled(bool)), ui->groupAdvanced, SLOT(setVisible(bool)) );
}

/**
 * Destrutor da janela.
 * Libera a memória alocada.
 */
GameOptions::~GameOptions()
{
    delete ui;
}

/**
 * Evento de clique no botão "Mover o jogador para cima".
 * Entra em modo de "escuta de teclas" para permitir que o usuário escolha.
 *
 * @param pressed Se o botão foi pressionado ou despressionado.
 */
void GameOptions::getMoveUpKey( bool pressed )
{
    if ( pressed ) {
        ui->btnMoveDown->setChecked( false );
        grabKeyboard();
        grabbingKey = BTN_MOVEUP;
    } else {
        releaseKeyboard();
        grabbingKey = -1;
    }
}

/**
 * Evento de clique no botão "Mover o jogador para baixo".
 * Entra em modo de "escuta de teclas" para permitir que o usuário escolha.
 *
 * @param pressed Se o botão foi pressionado ou despressionado.
 */
void GameOptions::getMoveDownKey( bool pressed )
{
    if ( pressed ) {
        ui->btnMoveUp->setChecked( false );
        grabKeyboard();
        grabbingKey = BTN_MOVEDOWN;
    } else {
        releaseKeyboard();
        grabbingKey = -1;
    }
}

/**
 * Evento de pressionamento de uma tecla.
 * Utilizado quando o jogador clica em um dos botões para escolher a tecla de
 * movimentação.
 *
 * @param e O evento de pressionamento da tecla.
 */
void GameOptions::keyPressEvent( QKeyEvent * event )
{
    // não aceita teclas especiais (Ctrl, Shift, ...)
    if ( Qt::NoModifier != event->modifiers() ) {
        event->ignore();
        return;
    }

    event->accept();

    if ( BTN_MOVEDOWN == grabbingKey ) {
        ui->btnMoveDown->setChecked( false );
        ui->btnMoveDown->setText( getKeyString( event->key() ) );
    }
    else if ( BTN_MOVEUP == grabbingKey ) {
        ui->btnMoveUp->setChecked( false );
        ui->btnMoveUp->setText( getKeyString( event->key() ) );
    }

    releaseKeyboard();
    grabbingKey = -1;
}

/**
 * Método auxiliar que retorna a representação gráfica de uma tecla.
 * Utilizado para exibir qual tecla foi pressionada pelo usuário.
 *
 * @param code O código da tecla pressionada.
 * @return Uma string que representa a tecla.
 */
QString GameOptions::getKeyString( int code )
{
    if ( code >= Qt::Key_Exclam && code <= Qt::Key_AsciiTilde ) {
        return QString( QChar( code ) );
    }

    QString str;

    switch ( code ) {
        case Qt::Key_Down:
            str = "Seta para baixo";
            break;
        case Qt::Key_Up:
            str = "Seta para cima";
            break;
        case Qt::Key_Left:
            str = "Seta para esquerda";
            break;
        case Qt::Key_Right:
            str = "Seta para direita";
            break;
        case Qt::Key_Space:
            str = QString::fromUtf8( "Barra de espaço" );
            break;
        case Qt::Key_Enter:
            str = "Enter";
            break;
        case Qt::Key_Backspace:
            str = "Backspace";
            break;
        // TODO: teclas, teclas, e mais teclas aqui...
        default:
            str = "<tecla desconhecida>";
            break;
    }

    return str;
}
