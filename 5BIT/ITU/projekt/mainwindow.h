#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "playercount.h"
#include "aboutscreen.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void menu_action();
    void menu_handler();
    void player_count();
    QMediaPlayer *music;

private slots:

    void on_menu_exit_clicked();

    void on_menu_about_clicked();

    void on_menu_play_clicked();

    void on_menu_mute_clicked();

private:
   Ui::MainWindow *ui;
   int active = 1;

};

#endif // MAINWINDOW_H
