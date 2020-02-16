#ifndef ABOUTSCREEN_H
#define ABOUTSCREEN_H

#include <QDialog>

namespace Ui {
class AboutScreen;
}

class AboutScreen : public QDialog
{
    Q_OBJECT

public:
    explicit AboutScreen(QWidget *parent);
    ~AboutScreen();
    void reject();

private slots:
    void on_pushButton_clicked();

    void on_btn_mute_clicked();

private:
    Ui::AboutScreen *ui;
};

#endif // ABOUTSCREEN_H
