#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <QDialog>
#include "game.h"

class QString;

namespace Ui
{
    class GameOptions;
}

/**
 * @class GameOptions gameoptions.h "gameoptions.h"
 * Diálogo de configurações do jogo.
 *
 * Representa uma janela do tipo QDialog que é utilizada para configurar o jogo:
 * o modo de jogo (servidor ou cliente), as teclas de controle do jogador, além
 * de um modo avançado onde é possível definir a porta serial na qual conectar.
 */
class GameOptions : public QDialog
{
    Q_OBJECT

public:
    explicit GameOptions( QWidget * parent = 0 );
    ~GameOptions();

private slots:
    void getMoveUpKey( bool pressed );
    void getMoveDownKey( bool pressed );
    void validateConfig();

private:
    void keyPressEvent( QKeyEvent * event );
    QString getKeyString( int code );

    Ui::GameOptions *ui;

    short int grabbingKey;

    const static short int BTN_MOVEUP   = 0;
    const static short int BTN_MOVEDOWN = 1;
};

#endif // GAMEOPTIONS_H
