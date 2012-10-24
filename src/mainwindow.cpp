#include <QMessageBox>
#include <QMainWindow>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"      // gerado pelo Qt a partir de mainwindow.ui
#include "game.h"
#include "gameoptions.h"

/**
 * Construtor.
 * Cria a janela e inicializa os itens do jogo (bola, jogadores, etc.).
 *
 * @param parent O pai dessa janela (normalmente o desktop).
 */
MainWindow::MainWindow( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    // carrega a interface
    this->ui->setupUi( this );

    this->game = NULL;
    this->op   = NULL;

    // conecta sinais e slots (eventos)
    connect( this->ui->actionNewGame, SIGNAL(triggered()), this, SLOT(startNewGame()) );
    connect( this->ui->actionQuit,    SIGNAL(triggered()), this, SLOT(close()) );
    connect( this->ui->actionAbout,   SIGNAL(triggered()), this, SLOT(about()) );

    this->startNewGame();
}

/**
 * Destrutor da janela.
 * Libera a memória alocada.
 */
MainWindow::~MainWindow()
{
    delete this->ui;
    delete this->game;
    delete this->op;
}

/**
 * Slot utilizado para exibir o diálogo sobre o aplicativo.
 *
 * Exibe uma caixa de diálogo com informações sobre o jogo e os seus
 * desenvolvedores.
 */
void MainWindow::about()
{
    QString msg = "";
    msg += "<h3>Serial Pong (Futebol Simplificado Multiplayer)</h3>";
    msg += "<h4>Desenvolvido por:</h4>";
    msg += "<ul><li>Eduardo Weiland</li><li>Wellington Camara Lopes</li></ul>";

    QMessageBox::about( this, "Sobre o jogo", msg );
}

void MainWindow::closeConfigDialog( int status )
{
    if ( QDialog::Accepted != status ) {
        this->op->close();
        this->op = NULL;
    }
}

/**
 * Slot utilizado para iniciar um novo jogo.
 *
 * É conectado ao evento triggered da ação de "Novo jogo", e é o responsável por
 * realizar todas as operações necessárias para iniciar uma nova partida.
 */
void MainWindow::startNewGame()
{
    // não pode iniciar um novo jogo se já existe um execução
    if ( NULL != this->game && this->game->isPlaying() ) {
        return;
    }

    // cria a tela de configurações
    if ( NULL == this->op ) {
        this->op = new GameOptions( this );
        op->show();
        connect( this->op, SIGNAL(accepted()),    this, SLOT(startNewGame()) );
        connect( this->op, SIGNAL(finished(int)), this, SLOT(closeConfigDialog(int)) );
        return;
    }

    if ( NULL != this->game ) {
        delete this->game;
        this->game = NULL;
    }

    op->close();

    this->game = new Game( this );
    this->ui->centralLayout->addWidget( this->game, 1 );
//    this->setCentralWidget( this->game );

    // carrega as configurações
    this->game->setPortName( this->op->getSerialPort() );
    this->game->setGameMode( this->op->getGameMode() );
    this->game->setMoveUpKeyCode( this->op->getMoveUpKey() );
    this->game->setMoveDownKeyCode( this->op->getMoveDownKey() );
    delete this->op;
    this->op = NULL;

    connect( this->ui->actionSpeedPlus,  SIGNAL(triggered()), this->game, SLOT(accelerate()) );
    connect( this->ui->actionSpeedMinus, SIGNAL(triggered()), this->game, SLOT(deaccelerate()) );

    // Vamos jogar!
    this->game->play();
}
