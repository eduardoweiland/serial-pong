#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

class Ball;
class QextSerialPort;
class QString;
class QTimer;
class Player;
class ScoreBoard;
class QGraphicsDropShadowEffect;

/**
 * BUG:
 *   NECESSÁRIO CONVERTER AS STRUCTS PARA CHAR* COM UMA FUNÇÃO ESPECÍFICA!
 *   APENAS USAR TYPE CAST NÃO SÓ PODE COMO ESTÁ DANDO PROBLEMAS!!
 */


/**
 * Define a estrutura utilizada na comunicação serial.
 *
 * Esse campo de bits é utilizado para definir os dados enviados do servidor
 * para o cliente pela comunicação serial.
 *
 * @note Essa estrutura ocupa no total 8 bytes ou 63 bits.
 */
typedef struct {
    // informações de posicionamento e informações do jogo
    signed   ballX        : 11; /**< Posição X da bola (de -15 até 1015 = 11 bits) */
    unsigned ballY        : 9;  /**< Posição Y da bola (de 0 até 500 = 9 bits) */
    unsigned playerLeft   : 9;  /**< Posição Y do jogador da esquerda (de 0 até 370 = 9 bits) */
    unsigned scoreLeft    : 6;  /**< Placar do jogador da esquerda (de 0 até 63 = 6 bits) */
    unsigned scoreRight   : 6;  /**< Placar do jogador da direita (de 0 até 63 = 6 bits) */
    unsigned gameSeconds  : 12; /**< Tempo de jogo em segundos (12 bits = 1h8min15s de jogo) */

    // informações de controle gerais
    unsigned ballRotation : 9;  /**< Rotação da bola (de 0 a 360 = 9 bits) */
    unsigned paused       : 1;  /**< Bit que indica se o jogo está pausado (1) ou não (0). */
    unsigned isGoal       : 1;  /**< Bit que indica se ocorreu um gol (para exibir a mensagem no cliente). */
} GameControl;

/**
 * Estrutura com informações do cliente.
 *
 * Durante o jogo, o lado cliente precisa enviar algumas informações para o
 * servidor (movimento do jogador, etc.). Essa estrutura define o formato dos
 * dados utilizados para essa comunicação.
 *
 * Da mesma forma que GameControl, essa estrutura é um campo de bits, e ocupa
 * 15 bits ou 2 bytes.
 */
typedef struct {
    unsigned playerPos : 9; /**< Posição Y do jogador da esquerda (de 0 até 370 = 9 bits) */
    unsigned velocity  : 6; /**< Velocidade da bola configurada no cliente (de 1 a 25 = 6 bits) */
} ClientInfo;

/**
 * Estrutura utilizada para controlar o início do jogo.
 *
 * Os dois jogadores ficam enviando e recebendo essa estrutura até que ambos
 * informem que estão pronto para o jogo (campo ready definido como true).
 *
 * Na prática, essa struct impede que um jogador possa jogar sozinho contra um
 * adversário paralisado em campo (sem receber as informações enviadas pelo
 * outro computador - servidor ou cliente).
 *
 * É enviado também o gameMode, necessário saber se a configuração do outro
 * jogador é compatível, para não iniciar o jogo com dois servidores ou dois
 * clientes.
 *
 * @note Essa estrutura ocupa 91 bits, ou 12 bytes.
 */
typedef struct {
    bool      ready;     /**< Flag que indica se o jogador está pronto para começar o jogo. */
    char      name[11];  /**< Nome do jogador (10 caracteres + '\0'). */
    short int gameMode;  /**< Flag que indica o modo de jogo configurado. */
} Greetings;

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
    void setMoveWithMouse( bool move );
    void setLocalPlayerName( QString name );
    void setRemotePlayerName( QString name );

    // getters
    QString  getPortName() const;
    GameMode getGameMode() const;
    Qt::Key  getMoveUpKeyCode () const;
    Qt::Key  getMoveDownKeyCode () const;
    bool     getMoveWithMouse() const;
    QString  getLocalPlayerName() const;
    QString  getRemotePlayerName() const;

    bool isPlaying() const;

public slots:
    void play();
    void readyToPlay();
    void accelerate();
    void deaccelerate();
    void pauseGame();
    void continueGame( bool center = true );
    bool showMessage( QString msg, int time = 5000 );
    void removeMessage();

private slots:
    void playOnServer();
    void playOnClient();
    void waitPlayer();

private:
    // configurações do jogo
    QString  portName;
    GameMode gameMode;
    Qt::Key  moveUpKeyCode;
    Qt::Key  moveDownKeyCode;
    bool     moveWithMouse;

    // controle do jogo
    QextSerialPort * port;
    QTimer         * timer;
    QTime          * gameTime;
    ScoreBoard     * scoreBoard;

    QGraphicsTextItem         * displayedText;
    QGraphicsDropShadowEffect * displayedTextEffect;

    unsigned short int player1score;
    unsigned short int player2score;

    bool otherReady;
    bool paused;
    int  speed;

    QString localPlayerName;
    QString remotePlayerName;

    // items do jogo
    QGraphicsRectItem * field;
    QGraphicsRectItem * goalLeft;
    QGraphicsRectItem * goalRight;
    Ball              * ball;
    Player            * player1;
    Player            * player2;

    void keyPressEvent( QKeyEvent * event );
    void resizeEvent( QResizeEvent * event );
    bool eventFilter( QObject * obj, QEvent * event );

    void configureSerialPort();
    void initializeConfig();
    bool verifyGoal();
    void playerCollision();
};

#endif // GAME_H
