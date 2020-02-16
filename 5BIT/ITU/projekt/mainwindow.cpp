#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QKeyEvent>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocus();
    menu_handler(); // paint menu buttons
    /* mute icon for mute button */
    QPixmap pix("mute.png");
    pix = pix.scaled(30,30);
    ui->menu_mute->setIcon(pix);
    ui->menu_mute->setIconSize(QSize(105, 105));
    /* set playlist to loop song */
    QDir dir;
    QMediaPlaylist *playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl::fromLocalFile(dir.absolutePath()+"/markalo.ogg")); //must include absolute path to audio file
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    /* play music */
    music = new QMediaPlayer();
    music->setPlaylist(playlist);
    music->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::player_count() {}

/* sets button text color according to selected one */
void MainWindow::menu_handler() {

    if (active == 1) {
        MainWindow::ui->menu_play->setStyleSheet("font: bold; color: #ffffff;");
        MainWindow::ui->menu_about->setStyleSheet("font: normal; color: #0b93a5;");
        MainWindow::ui->menu_exit->setStyleSheet("font: normal; color: #0b93a5;");
    }
    if (active == 2) {
        MainWindow::ui->menu_play->setStyleSheet("font: normal; color: #0b93a5;");
        MainWindow::ui->menu_about->setStyleSheet("font: bold; color: #ffffff;");
        MainWindow::ui->menu_exit->setStyleSheet("font: normal; color: #0b93a5;");
    }
    if (active == 3) {
        MainWindow::ui->menu_play->setStyleSheet("font: normal; color: #0b93a5;");
        MainWindow::ui->menu_about->setStyleSheet("font: normal; color: #0b93a5;");
        MainWindow::ui->menu_exit->setStyleSheet("font: bold; color: #ffffff;");
    }

}
/* decides what to do when enter pressed */
void MainWindow::menu_action() {
    if (active == 1)
        ui->menu_play->animateClick();
    if (active == 2)
        ui->menu_about->animateClick();
    if (active == 3)
        ui->menu_exit->animateClick();
}


void MainWindow::on_menu_exit_clicked()
{
    active = 3;
    menu_handler();
    qDebug() << "Exiting game...";
    QApplication::quit();

}

/* play game -> move to player count selection */
void MainWindow::on_menu_play_clicked()
{
    active = 1;
    menu_handler();
    PlayerCount window(this);
    this->hide();
    window.exec();
    this->show();
    this->setFocus();
}

/* show about screen */
void MainWindow::on_menu_about_clicked()
{
    active = 2;
    menu_handler();
    AboutScreen about(this);
    about.exec();
    qDebug() << "This is Patrick...";
    this->setFocus();
}

/* loops menu items, enter/return action */
void MainWindow::keyPressEvent(QKeyEvent *event)
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




void MainWindow::on_menu_mute_clicked()
{
    if (music->isMuted())
        music->setMuted(false);
    else
        music->setMuted(true);
}
