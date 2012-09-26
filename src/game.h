#ifndef GAME_H
#define GAME_H

class QString;
class QextSerialPort;

/**
 * Define a estrutura utilizada na comunicação serial.
 * Esse campo de bits é utilizado para definir os dados enviados pela
 * comunicação serial.
 * @note Essa estrutura ocupa no total 8 bytes ou 64 bits.
 */
typedef struct {
    unsigned ballX       : 10; /**< Posição X da bola (de 0 até 1000 = 10 bits) */
    unsigned ballY       : 9;  /**< Posição Y da bola (de 0 até 500 = 9 bits) */
    unsigned playerLeft  : 9;  /**< Posição Y do jogador da esquerda (de 0 até 500) */
    unsigned playerRight : 9;  /**< Posição Y do jogador da direita (de 0 até 500) */
    unsigned scoreLeft   : 7;  /**< Placar do jogador da esquerda (de 0 até 127 = 7 bits) */
    unsigned scoreRight  : 7;  /**< Placar do jogador da direita (de 0 até 127 = 7 bits) */
    unsigned gameSeconds : 13; /**< Tempo de jogo em segundos (13 bits = 2h16min31s de jogo) */
} GameControl;

/**
 * @class Game game.h "game.h"
 * Representa uma instância do jogo.
 *
 * Essa classe é responsável por controlar todo o jogo e realizar a comunicação
 * serial entre os computadores.
 *
 * @todo implementar herança de QGraphicsScene ? a analisar...
 */
class Game
{
public:
    Game();
    ~Game();

    /**
     * Possíveis modos de jogo.
     */
    enum GameMode {
        SERVER, /**< O jogo será iniciado no modo servidor. */
        CLIENT  /**< O jogo será iniciado no modo cliente. */
    };

    // setters
    void setPortName( QString port );
    void setGameMode( GameMode mode );

    // getters
    QString getPortName() const;
    GameMode getGameMode() const;

public slots:
    void play();

private:
    QString portName;
    GameMode gameMode;

    QextSerialPort * port;

    void configureSerialPort();
};

#endif // GAME_H
