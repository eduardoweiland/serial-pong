#include <QTimer>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ball.h"
#include "qextserialport.h"

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


    port = new QextSerialPort( "/dev/ttyS0" );
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
}

/**
 * Slot utilizado para aumentar a velocidade da bola.
 */
void MainWindow::accelerateBall()
{
    ball->accelerate();
}

/**
 * Slot utilizado para diminuir a velocidade da bola.
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
