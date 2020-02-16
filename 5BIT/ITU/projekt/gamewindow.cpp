#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "gamecontroller.h"
#include <QThread>

GameWindow::GameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    GameFinish finish;
    //this->hide();
    this->close();
    finish.exec();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::setControls(char **cntrl) {
    controls = cntrl;
}

void GameWindow::gameLoop () {

}
