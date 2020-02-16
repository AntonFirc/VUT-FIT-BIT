#ifndef PLAYERCOUNT_H
#define PLAYERCOUNT_H

#include <QDialog>
#include "controlsselect.h"

namespace Ui {
class PlayerCount;
}

class PlayerCount : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerCount(QWidget *parent);
    ~PlayerCount();
    int player_count = 1;
    void reject();

private slots:
    void on_pushButton_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_btn_mute_clicked();

private:
    Ui::PlayerCount *ui;
};

#endif // PLAYERCOUNT_H
