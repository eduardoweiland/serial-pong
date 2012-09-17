#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QGraphicsRectItem;
class QGraphicsScene;
class QGraphicsView;
class QTimer;
class Ball;
class QextSerialPort;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget * parent = 0 );
    ~MainWindow();

private slots:
    void about();
    void accelerateBall();
    void deaccelerateBall();

private:
    Ui::MainWindow * ui;
    QTimer * timer;

    QGraphicsScene * scene;
    QGraphicsRectItem * field;

    Ball * ball;
    QextSerialPort * port;
};

#endif // MAINWINDOW_H
