#ifndef GAMEFINISH_H
#define GAMEFINISH_H

#include <QDialog>

namespace Ui {
class GameFinish;
}

class GameFinish : public QDialog
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    explicit GameFinish(QWidget *parent);
    ~GameFinish();
    void menu_action();
    void menu_handler();
    void reject();

private slots:
    void on_pushButton_3_clicked();

    void on_btn_exit_clicked();

    void on_btn_mernu_clicked();

    void on_btn_play_clicked();

    void on_btn_mute_clicked();

private:
    Ui::GameFinish *ui;
    int active = 1;
};

#endif // GAMEFINISH_H
