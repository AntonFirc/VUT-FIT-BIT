#ifndef CONTROLSSELECT_H
#define CONTROLSSELECT_H
#include "gamewindow.h"
#include <QDialog>
#include "snake.h"

namespace Ui {
class ControlsSelect;
}

class ControlsSelect : public QDialog
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    explicit ControlsSelect(QWidget *parent);
    void setPlayerCount(int cnt);
    void setControls(int idx);
    ~ControlsSelect();
    int controls[10][3];
    void reject();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_btn_mute_clicked();

private:
    Ui::ControlsSelect *ui;
    int btn = 1;
    int player_count;
    int current_player = 1;
    bool capture = true;
    bool mouse = false;
    bool found = false;
    //Snake had;
};

#endif // CONTROLSSELECT_H
