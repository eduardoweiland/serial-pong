#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <QDialog>

namespace Ui
{
    class GameOptions;
}

class GameOptions : public QDialog
{
    Q_OBJECT

public:
    explicit GameOptions(QWidget *parent = 0);
    ~GameOptions();

private slots:
    void getMoveUpKey( bool pressed );
    void getMoveDownKey( bool pressed );

private:
    void keyPressEvent( QKeyEvent * e );

    Ui::GameOptions *ui;

    short int grabbingKey;

    const static short int BTN_MOVEUP   = 0;
    const static short int BTN_MOVEDOWN = 1;
};

#endif // GAMEOPTIONS_H
