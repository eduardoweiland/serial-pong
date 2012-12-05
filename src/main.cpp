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
 * devem funcionar (considerando um ambiente Linux/Unix):
 *
 *      $ cd /path/to/serial-pong
 *      $ qmake serial-pong.pro   ## dependendo do ambiente o comando pode ser qmake-qt4
 *      $ make
 *      $ ./serial-pong
 *
 * Se você está em um ambiente Windows, então você realmente deve utilizar o
 * Qt Creator.
 *
 * @note __IMPORTANTE__
 *       O jogo __deve__ ser compilado no modo Release para que a comunicação
 *       serial funcione corretamente.
 *
 * ### Executando
 *
 * Para poder executar o aplicativo são necessárias as bibliotecas dinâmicas do
 * Qt. São necessárias apenas QtCore e QtGui. Se o QtCreator está instalado,
 * então essas bibliotecas também devem estar. Para sistemas Windows, as
 * bibliotecas necessárias já estão incluídas.
 */

#include <QtGui/QApplication>
#include <QTime>
#include <QFontDatabase>
#include <QMessageBox>

#include "mainwindow.h"
#include "globals.h"

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
    app.setApplicationVersion( "1.0" );
    app.setOrganizationName( "Eduardo & Wellington softwares" ); // :D

    // carrega a fonte utilizada no placar
    QFontDatabase::addApplicationFont( ":/fonts/erbos_draco_nbp.ttf" );

    // exibe a janela principal maximizada
    MainWindow win;
    win.showMaximized();

    // Loop principal: só sai quando a janela for fechada
    int retCode = app.exec();

    switch ( retCode ) {
        case ERR_SERIAL_ERROR:
            QMessageBox::critical( &win, "Erro", QString::fromUtf8(
                "Erro ERR_SERIAL_ERROR:<br>"
                "Ocorreu um erro na comunicação serial.<br>"
                "Verifique as configurações e tente novamente.") );
            break;
        case ERR_BAD_GAME_MODE:
            QMessageBox::critical( &win, "Erro", QString::fromUtf8(
                "Erro ERR_BAD_GAME_MODE:<br>"
                "A configuração do jogo está incorreta.<br>"
                "Verifique as opções e tente novamente.") );
            break;
    }

    return retCode;
}
