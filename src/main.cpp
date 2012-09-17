/**
 * Trabalho de Programação Orientada a Objetos I
 * Serial Pong (Futebol Simplificado)
 *
 * Eduardo Weiland
 * Wellington Camara Lopes
 */

#include <QtGui/QApplication>
#include <QTime>

#include "mainwindow.h"
#include "gameoptions.h"

/**
 * Função main.
 * Responsável por criar a janela principal do jogo.
 *
 * @param argc Número de argumentos recebidos pela linha de comando.
 * @param argv Um vetor contendo os parâmetros recebidos. O programa aceita as
 *             opções padrão para softwares desenvolvidos com Qt.
 * @return Código de saída para o sistema operacional. Pode indicar um erro ou
 *         saída com sucesso.
 * @see http://qt-project.org/doc/qt-4.8/qapplication.html#QApplication
 */
int main( int argc, char ** argv )
{
    // inicializa a semente de números aleatórios
    qsrand( QTime(0,0,0).secsTo( QTime::currentTime() ) );

    QApplication app( argc, argv );
    app.setApplicationName( "Serial Pong" );
    app.setApplicationVersion( "0.0.0.0.0.0.1 pre-pre-pre-alpha" );
    app.setOrganizationName( "Eduardo & Wellington softwares" ); // :D

    // exibe a janela principal maximizada
    MainWindow win;
    win.showMaximized();

    GameOptions * go = new GameOptions();
    go->show();

    return app.exec();
}
