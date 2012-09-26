#include <QMessageBox>
#include <QTimer>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
#include "globals.h"

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
    ui->setupUi( this );

    // conecta sinais e slots (eventos)
    connect( ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()) );
    connect( ui->actionSpeedPlus, SIGNAL(triggered()), this, SLOT(accelerateBall()) );
    connect( ui->actionSpeedMinus, SIGNAL(triggered()), this, SLOT(deaccelerateBall()) );
    connect( ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()) );

    // cria a cena para conter todos os itens do jogo.
    scene = new QGraphicsScene( 0, 0, 1000, 500 );
    scene->setBackgroundBrush( Qt::black );

    // o campo do jogo (a bola não deve sair dele)
    field = new QGraphicsRectItem( 0, 0, 1000, 500 );
    field->setBrush( Qt::darkGreen );
    scene->addItem( field );

    // cria a bola e adiciona na cena
    ball = new Ball();
    ball->setPos( scene->width() / 2, scene->height() / 2 );
    scene->addItem( ball );

    ui->graphicsView->setScene( scene );

    // inicializa o contador de frames
    // TODO: esperar o jogo começar para fazer isso
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), scene, SLOT(advance()) );
    timer->start( 1000 / 20 );  // 20 FPS
}

/**
 * Destrutor da janela.
 * Libera a memória alocada.
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete field;
    delete ball;
    delete scene;
}

/**
 * Slot utilizado para aumentar a velocidade da bola.
 * Recebe os eventos tanto do botão quanto da tecla de atalho.
 */
void MainWindow::accelerateBall()
{
    ball->accelerate();
}

/**
 * Slot utilizado para diminuir a velocidade da bola.
 * Recebe os eventos tanto do botão quanto da tecla de atalho.
 */
void MainWindow::deaccelerateBall()
{
    ball->deaccelerate();
}

/**
 * Slot utilizado para exibir o diálogo sobre o aplicativo.
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
