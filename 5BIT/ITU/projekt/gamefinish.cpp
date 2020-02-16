#include "gamefinish.h"
#include "ui_gamefinish.h"
#include "mainwindow.h"
#include <QDebug>
#include <QKeyEvent>
#include "snake.h"

GameFinish::GameFinish(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameFinish)
{
    ui->setupUi(this);
    this->setFocus();
    menu_handler();

    /* mute image for mute button*/
    QPixmap mute("mute.png");
    mute = mute.scaled(30,30);
    ui->btn_mute->setIcon(mute);
    ui->btn_mute->setIconSize(QSize(105, 105));
}


GameFinish::~GameFinish()
{
    delete ui;
}

void GameFinish::on_btn_exit_clicked()
{
    QApplication::quit();
}

void GameFinish::on_btn_mernu_clicked()
{
    /* return 0 = dont restart game, back to menu */
    this->done(0);
}

void GameFinish::on_btn_play_clicked()
{
    /* returns 1 = restart game */
    this->done(1);
}

/* sets button text color according to selected one */
void GameFinish::menu_handler() {

    if (active == 1) {
        ui->btn_play->setStyleSheet("font: bold; color: #ffffff;");
        ui->btn_mernu->setStyleSheet("font: normal; color: #0b93a5;");
        ui->btn_exit->setStyleSheet("font: normal; color: #0b93a5;");
    }
    if (active == 2) {
        ui->btn_play->setStyleSheet("font: normal; color: #0b93a5;");
        ui->btn_mernu->setStyleSheet("font: bold; color: #ffffff;");
        ui->btn_exit->setStyleSheet("font: normal; color: #0b93a5;");
    }
    if (active == 3) {
        ui->btn_play->setStyleSheet("font: normal; color: #0b93a5;");
        ui->btn_mernu->setStyleSheet("font: normal; color: #0b93a5;");
        ui->btn_exit->setStyleSheet("font: bold; color: #ffffff;");
    }

}

/* decides what to do when enter pressed */
void GameFinish::menu_action() {
    if (active == 1)
        ui->btn_play->animateClick();
    if (active == 2)
        ui->btn_mernu->animateClick();
    if (active == 3)
        ui->btn_exit->animateClick();
}

/* loops menu items, enter/return action */
void GameFinish::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        qDebug() << "Enter";
        menu_action();
    }
    else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
        qDebug() << "Down";
        if (active == 1)
            active = 3;
        else
            active--;
        qDebug() << active;
    }
    else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
        qDebug() << "Up";
        if (active == 3)
            active = 1;
        else
            active++;
        qDebug() << active;
    }
    else
        qDebug() << "No enter";

    menu_handler();
}

/* mute button clicked */
void GameFinish::on_btn_mute_clicked()
{
    if (((MainWindow*)(parent()->parent()->parent()))->music->isMuted())
        ((MainWindow*)(parent()->parent()->parent()))->music->setMuted(false);
    else
        ((MainWindow*)(parent()->parent()->parent()))->music->setMuted(true);
}

void GameFinish::reject()
{
    QApplication::quit();
}
