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
};

#endif // GAME_H
