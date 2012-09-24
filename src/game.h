#ifndef GAME_H
#define GAME_H

class QString;
class QextSerialPort;

class Game
{
public:
    Game();
    ~Game();

    enum GameMode {
        SERVER,
        CLIENT
    };

    // setters
    void setPortName( QString port );
    void setGameMode( GameMode mode );

    // getters
    QString getPortName() const;
    QString getGameMode() const;

public slots:
    void play();

private:
    QString portName;
    GameMode gameMode;

    QextSerialPort * port;

    void configureSerialPort();

    // campo de bits utilizado na comunicação serial (total = 8 bytes)
    struct {
        unsigned ballX       : 10; // posição X da bola (de 0 até 1000 = 10 bits)
        unsigned ballY       : 9;  // posição Y da bola (de 0 até 500 = 9 bits)
        unsigned playerLeft  : 9;  // posição Y do jogador da esquerda (de 0 até 500)
        unsigned playerRight : 9;  // posição Y do jogador da direita (de 0 até 500)
        unsigned scoreLeft   : 7;  // placar do jogador da esquerda (de 0 até 127 = 7 bits)
        unsigned scoreRight  : 7;  // placar do jogador da direita (de 0 até 127 = 7 bits)
        unsigned gameSeconds : 13; // tempo de jogo (em segundos - 13 bits = 2h16min31s de jogo)
    } gameControl;
};

#endif // GAME_H
