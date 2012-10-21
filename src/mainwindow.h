#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Game;
class GameOptions;
class ScoreBoard;

namespace Ui
{
    class MainWindow;
}

/**
 * @class MainWindow mainwindow.h "mainwindow.h"
 * Janela principal do jogo.
 *
 * Essa classe define a interface principal do jogo, a janela, os menus e as
 * barras de ferramentas. A interface é carregada do arquivo XML criado com o
 * QtDesigner (integrado à IDE QtCreator). Os atalhos do teclado também são
 * definidos na interface.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget * parent = 0 );
    ~MainWindow();

private slots:
    void about();
    void closeConfigDialog( int status );
    void startNewGame();

private:
    Ui::MainWindow * ui;
    Game * game;
    GameOptions * op;
    ScoreBoard * scoreBoard;
};

#endif // MAINWINDOW_H
