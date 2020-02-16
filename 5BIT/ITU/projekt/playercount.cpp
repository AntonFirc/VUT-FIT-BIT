#include "playercount.h"
#include "ui_playercount.h"
#include "mainwindow.h"

#include <QDebug>

PlayerCount::PlayerCount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerCount)
{
    ui->setupUi(this);
    /* set icon for mute button */
    QPixmap pix("mute.png");
    pix = pix.scaled(30,30);
    ui->btn_mute->setIcon(pix);
    ui->btn_mute->setIconSize(QSize(105, 105));
}

PlayerCount::~PlayerCount()
{
    delete ui;
}

/* set player count and move to controls selection */
void PlayerCount::on_pushButton_clicked()
{
    ControlsSelect select(this);
    select.setPlayerCount(player_count);
    this->hide();
    select.exec();
    this->close();
}

/* when player count value changes update it */
void PlayerCount::on_spinBox_valueChanged(int arg1)
{
    player_count = arg1;
}

/* mute button clicked */
void PlayerCount::on_btn_mute_clicked()
{
    if (((MainWindow*)(parent()))->music->isMuted())
        ((MainWindow*)(parent()))->music->setMuted(false);
    else
        ((MainWindow*)(parent()))->music->setMuted(true);
}

void PlayerCount::reject()
{
    QApplication::quit();
}
