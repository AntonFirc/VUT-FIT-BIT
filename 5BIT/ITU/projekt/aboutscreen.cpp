#include "aboutscreen.h"
#include "ui_aboutscreen.h"
#include "mainwindow.h"

AboutScreen::AboutScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutScreen)
{
    ui->setupUi(this);
    /* set muteicon to mute button */
    QPixmap mute("mute.png");
    mute = mute.scaled(30,30);
    ui->btn_mute->setIcon(mute);
    ui->btn_mute->setIconSize(QSize(105, 105));
}

AboutScreen::~AboutScreen()
{
    delete ui;
}

/* button OK - exit to main menu */
void AboutScreen::on_pushButton_clicked()
{
    this->close();
}

void AboutScreen::on_btn_mute_clicked()
{
    if (((MainWindow*)(parent()))->music->isMuted())
        ((MainWindow*)(parent()))->music->setMuted(false);
    else
        ((MainWindow*)(parent()))->music->setMuted(true);
}

void AboutScreen::reject()
{
    QApplication::quit();
}
