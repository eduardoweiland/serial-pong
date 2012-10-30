#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

class Ball;
class QextSerialPort;
class QString;
class QTimer;
class Player;
class ScoreBoard;

/**
 * Define a estrutura utilizada na comunicação serial.
 *
 * Esse campo de bits é utilizado para definir os dados enviados do servidor
 * para o cliente pela comunicação serial.
 *
 * @note Essa estrutura ocupa no total 8 bytes ou 64 bits.
 */
typedef struct {
    unsigned ballX       : 10; /**< Posição X da bola (de 0 até 1000 = 10 bits) */
    unsigned ballY       : 9;  /**< Posição Y da bola (de 0 até 500 = 9 bits) */
    unsigned playerLeft  : 9;  /**< Posição Y do jogador da esquerda (de 0 até 370 = 9 bits) */
    unsigned playerRight : 9;  /**< Posição Y do jogador da direita (de 0 até 370 = 9 bits) */
    unsigned scoreLeft   : 7;  /**< Placar do jogador da esquerda (de 0 até 127 = 7 bits) */
    unsigned scoreRight  : 7;  /**< Placar do jogador da direita (de 0 até 127 = 7 bits) */
    unsigned gameSeconds : 13; /**< Tempo de jogo em segundos (13 bits = 2h16min31s de jogo) */
} GameControl;

typedef struct {
    signed playerMovement : 10; /**< Quantidade de pixels que o jogador cliente se moveu */
} ClientInfo;

/**
 * @class Game game.h "game.h"
 * Representa uma instância do jogo.
 *
 * Essa classe é responsável por controlar todo o jogo e realizar a comunicação
 * serial entre os computadores.
 */
class Game : public QGraphicsView
{
    Q_OBJECT

public:
    explicit Game( QWidget * parent = 0 );
    ~Game();

    /**
     * Possíveis modos de jogo.
     */
    enum GameMode {
        SERVER, /**< O jogo será iniciado no modo servidor. */
        CLIENT, /**< O jogo será iniciado no modo cliente. */
        UNKNOWN /**< Modo desconhecido. Usado para indicar algum erro ou configuração incompleta. */
    };

    // setters
    void setPortName( QString port );
    void setGameMode( GameMode mode );
    void setMoveUpKeyCode ( Qt::Key key );
    void setMoveDownKeyCode ( Qt::Key key );

    // getters
    QString  getPortName() const;
    GameMode getGameMode() const;
    Qt::Key  getMoveUpKeyCode ();
    Qt::Key  getMoveDownKeyCode ();

    bool isPlaying() const;
    bool showMessage( QString msg, int time = 5000 );

public slots:
    void play();
    void accelerate();
    void deaccelerate();

private slots:
    void playOnServer();
    void playOnClient();
    void removeMessage();

private:
    // configurações do jogo
    QString  portName;
    GameMode gameMode;
    Qt::Key  moveUpKeyCode;
    Qt::Key  moveDownKeyCode;

    // controle do jogo
    QextSerialPort    * port;
    QTimer            * timer;
    QTime             * gameTime;
    ScoreBoard        * scoreBoard;
    QGraphicsTextItem * displayedText;

    // items do jogo
    QGraphicsRectItem * field;
    Ball              * ball;
    Player            * player1;
    Player            * player2;

    void keyPressEvent( QKeyEvent * event );
    void configureSerialPort();
    void initializeConfig();
    void playerCollision();
};

#endif // GAME_H
