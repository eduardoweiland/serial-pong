#include "gameoptions.h"
#include "ui_gameoptions.h"

#include <QKeyEvent>

GameOptions::GameOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameOptions)
{
    ui->setupUi(this);

    connect( ui->btnMoveUp,   SIGNAL(toggled(bool)), this, SLOT(getMoveUpKey(bool)) );
    connect( ui->btnMoveDown, SIGNAL(toggled(bool)), this, SLOT(getMoveDownKey(bool)) );
}

GameOptions::~GameOptions()
{
    delete ui;
}

void GameOptions::getMoveUpKey( bool pressed )
{
    if (pressed) {
        this->grabKeyboard();
        this->grabbingKey = this->BTN_MOVEUP;
    } else {
        this->releaseKeyboard();
        this->grabbingKey = -1;
    }
}

void GameOptions::getMoveDownKey( bool pressed )
{
    if (pressed) {
        this->grabKeyboard();
        this->grabbingKey = this->BTN_MOVEDOWN;
    } else {
        this->releaseKeyboard();
        this->grabbingKey = -1;
    }
}

void GameOptions::keyPressEvent( QKeyEvent * e )
{
    e->accept();

    if ( this->grabbingKey == this->BTN_MOVEDOWN ) {
        ui->btnMoveDown->setChecked( false );
        ui->btnMoveDown->setText( QChar( e->key() ) );
    }
    else if ( this->grabbingKey == this->BTN_MOVEUP ) {
        ui->btnMoveUp->setChecked( false );
        ui->btnMoveUp->setText( QChar( e->key() ) );
    }

    this->releaseKeyboard();
    this->grabbingKey = -1;
}
