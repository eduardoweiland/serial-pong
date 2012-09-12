#include <QMessageBox>
#include <QTimer>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
#include "qextserialport.h"
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
    // carrega a interface e conecta os eventos
    ui->setupUi( this );

    // conecta sinais e slots
    connect( ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()) );
    connect( ui->actionVelocityPlus, SIGNAL(triggered()), this, SLOT(accelerateBall()) );
    connect( ui->actionVelocityMinus, SIGNAL(triggered()), this, SLOT(deaccelerateBall()) );
    connect( ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()) );

    // cria a cena para conter todos os itens do jogo.
    scene = new QGraphicsScene( 0, 0, 1100, 500 );
    scene->setBackgroundBrush( Qt::black );

    // o campo do jogo (a bola não deve sair dele)
    field = new QGraphicsRectItem( 0, 0, 1100, 500 );
    field->setBrush( Qt::darkGreen );
    scene->addItem( field );

    // cria a bola e adiciona na cena
    ball = new Ball();
    ball->setPos( 550, 250 );   // TODO: tem jeito melhor de centralizar?
    scene->addItem( ball );

    ui->graphicsView->setScene( scene );
    //ui->graphicsView->adjustSize();

    // inicializa o contador de frames
    // TODO: esperar o jogo começar para fazer isso
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), scene, SLOT(advance()) );
    timer->start( 1000 / 20 );  // 20 FPS


    port = new QextSerialPort( SERIALPORT );
    connect( port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()) );
    port->open( QIODevice::ReadWrite );
}

/**
 * Destrutor da janela.
 * Libera a memória alocada.
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete scene;
    delete field;
    delete ball;
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

void MainWindow::onDataAvailable()
{
    QByteArray data = port->readAll();
    qDebug() << data;
}

/**
 * Slot utilizado para exibir o diálogo sobre o aplicativo.
 */
void MainWindow::about()
{
    QString msg = "";
    msg += "<h3>Serial Pong (Futebol Simplificado Multiplayer)</h3>";
    msg += "<h4>Desenvolvido por:</h4>";
    msg += "<ul><li>Eduardo Weiland</li><li>Wellington Camara Lopes</li></ul>";

    QMessageBox::about( this, "Sobre o jogo", msg );
}
