/**
 * @mainpage Serial Pong (Futebol Simplificado Multiplayer)
 *
 * ### Sobre
 *
 * Serial Pong (ou Futebol Simplificado Multiplayer) é um trabalho desenvolvido
 * na disciplina de Programação Orientada a Objetos I. Trata-se de um jogo de
 * futebol simplificado, para apenas dois jogadores (bem semelhante ao clássico
 * jogo pong).
 *
 * ### Autores
 *
 * Esse trabalho foi desenvolvido por:
 * - Eduardo Weiland
 * - Wellington Camara Lopes
 *
 * ### Compilando
 *
 * O jogo utiliza o [framework Qt](http://qt-project.org/) para construir a
 * interface gráfica. Para compilar o jogo é necessário instalar as ferramentas
 * de desenvolvimento disponíveis para download no site oficial. É recomendável
 * utilizar o Qt Creator.
 *
 * Se não quiser (ou não puder) utilizar o Qt Creator, os seguintes comandos
 * devem funcionar (considerando um ambiente Unix):
 *
 *      $ cd /path/to/serial-pong
 *      $ qmake serial-pong.pro
 *      $ make
 *      # make install
 *      $ serial-pong
 *
 * Se você está em um ambiente Windows, então você realmente deve utilizar o
 * Qt Creator.
 *
 * ### Executando
 *
 * Para poder executar o aplicativo são necessárias as bibliotecas dinâmicas do
 * Qt. São necessárias apenas QtCore e QtGui. Se o QtCreator está instalado,
 * então essas bibliotecas também devem estar.
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
