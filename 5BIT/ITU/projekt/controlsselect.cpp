#include "controlsselect.h"
#include "ui_controlsselect.h"
#include <QDebug>
#include <QKeyEvent>
#include <stdio.h>
#include <stdlib.h>
#include "mainwindow.h"



ControlsSelect::ControlsSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlsSelect)
{
    ui->setupUi(this);
    /* set "button-background" images */
    QPixmap pix("keyboard-button-md.png");
    pix = pix.scaled(180,180);
    ui->img_right->setPixmap(pix);
    ui->img_left->setPixmap(pix);
    ui->img_action->setPixmap(pix);
    /* set text for player 1, hide error text */
    ui->lbl_player->setText("Player 1");
    ui->err_assigned->setVisible(false);

    /* mute button image */
    QPixmap mute("mute.png");
    mute = mute.scaled(30,30);
    ui->btn_mute->setIcon(mute);
    ui->btn_mute->setIconSize(QSize(105, 105));
}

ControlsSelect::~ControlsSelect()
{
    delete ui;
}

/* sets count of players */
void ControlsSelect::setPlayerCount(int cnt) {
    player_count = cnt;
}

void ControlsSelect::setControls(int idx) {
    /* Prepare for next player control selection
        change texts to default, */
    if (idx <= player_count) {
        ui->lbl_player->setText("Player " + QString::number(idx));
        ui->btn_left->setText("?");
        ui->btn_right->setText("?");
        ui->btn_action->setText("?");
    }
    /* start game if all players are set */
    else {
        //start game
        qDebug() << "Starting game";
        GameFinish game(this);
        this->hide();
        int i = 1;
        /* based on return code of end game screen, start game again
            if play again clicked -> i = 1 else i = 0*/
        while (i) {
            Snake had(this);
            had.exec();
            i = game.exec();
        }
        this->close();
    }
}

/* enter = confirm player controls selection
   key = set control*/
void ControlsSelect::keyPressEvent(QKeyEvent *event) {
    if (capture) {
        if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            qDebug() << "Enter";
            if (btn == 3) {
                if (mouse == true) {
                    ui->checkBox->setChecked(false);
                    ui->checkBox->setCheckable(false);
                    ui->checkBox->setEnabled(false);
                }
                btn = 1;
                setControls(++current_player);
            }
        }
        else {
            if (btn == 1) {
                ui->btn_left->setText(QString( QChar(event->key()) ));
                /* checks if button not assigned yet */
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 3; j++) {
                        if (event->key() == controls[i][j] || event->key() == Qt::Key_M)
                            found = true;
                    }
                }
                /* shows error line / hides error line and assigns button */
                if (found) {
                    ui->err_assigned->setVisible(true);
                    found = false;
                }
                else {
                    ui->err_assigned->setVisible(false);
                    controls[current_player - 1][0] = event->key();
                    btn = 2;
                }
            }
            else if (btn == 2) {
                ui->btn_right->setText(QString( QChar(event->key()) ));
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 3; j++) {
                        if (event->key() == controls[i][j])
                            found = true;
                    }
                }
                if (found) {
                    ui->err_assigned->setVisible(true);
                    found = false;
                }
                else {
                    ui->err_assigned->setVisible(false);
                    controls[current_player - 1][1] = event->key();
                    qDebug() << controls[current_player - 1][1];
                    btn = 3;
                }
            }
            else if (btn == 3) {
                ui->btn_action->setText(QString( QChar(event->key()) ));
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 3; j++) {
                        if (event->key() == controls[i][j])
                            found = true;
                    }
                }
                if (found) {
                    ui->err_assigned->setVisible(true);
                    found = false;
                }
                else {
                    ui->err_assigned->setVisible(false);
                    controls[current_player - 1][2] = event->key();
                }
            }
        }
    }

}

/* indicates that mouse will be used to control player */
void ControlsSelect::on_checkBox_stateChanged(int arg1)
{
    if (arg1) {
        btn = 3;
        mouse = true;
    }
}

/* mute button clicked */
void ControlsSelect::on_btn_mute_clicked()
{
    if (((MainWindow*)(parent()->parent()))->music->isMuted())
        ((MainWindow*)(parent()->parent()))->music->setMuted(false);
    else
        ((MainWindow*)(parent()->parent()))->music->setMuted(true);
}

void ControlsSelect::reject()
{
    QApplication::quit();
}
