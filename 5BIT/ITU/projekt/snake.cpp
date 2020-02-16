#include <QPainter>
#include <QTime>
#include "snake.h"
#include <QDebug>
#include <ctype.h>
#include "controlsselect.h"
#include "mainwindow.h"
#include <QApplication>

Snake::Snake(QWidget *parent) : QDialog(parent) {

    setStyleSheet("background-color:black;");
    leftDirection = false;
    rightDirection = true;
    upDirection = false;
    downDirection = false;
    inGame = true;

    /* set window size and start game */
    resize(B_WIDTH, B_HEIGHT);
    loadImages();
    initGame();
}

/* load snake images */
void Snake::loadImages() {

    dot.load("dot.png");
    dot = dot.scaled(10,10);
    head.load("head.png");
    head = head.scaled(10,10);
    apple.load("apple.png");
    apple = apple.scaled(10,10);
}

/* starts game, print snake and creates apple, starts movement timer */
void Snake::initGame() {

    dots = 3;

    for (int z = 0; z < dots; z++) {
        x[z] = 50 - z * 10;
        y[z] = 50;
    }

    locateApple();

    timerId = startTimer(DELAY);
}

void Snake::paintEvent(QPaintEvent *e) {

    Q_UNUSED(e);

    doDrawing();
}

/* draw snake head and all body-dots */
void Snake::doDrawing() {

    QPainter qp(this);

    if (inGame) {

        qp.drawImage(apple_x, apple_y, apple);

        for (int z = 0; z < dots; z++) {
            if (z == 0) {
                qp.drawImage(x[z], y[z], head);
            } else {
                qp.drawImage(x[z], y[z], dot);
            }
        }

    } else {

        gameOver();
    }
}


/* closes window on game over */
void Snake::gameOver() {

    this->close();

}

/* snake "ate" apple */
void Snake::checkApple() {

    if ((x[0] == apple_x) && (y[0] == apple_y)) {

        dots++;
        locateApple();
    }
}

/* compute new head coordinates and shift snake array */
void Snake::move() {

    for (int z = dots; z > 0; z--) {
        x[z] = x[(z - 1)];
        y[z] = y[(z - 1)];
    }

    if (leftDirection) {
        x[0] -= DOT_SIZE;
    }

    if (rightDirection) {
        x[0] += DOT_SIZE;
    }

    if (upDirection) {
        y[0] -= DOT_SIZE;
    }

    if (downDirection) {
        y[0] += DOT_SIZE;
    }
}

/* if collides with itself or border, end game */
void Snake::checkCollision() {

    for (int z = dots; z > 0; z--) {

        if ((z > 4) && (x[0] == x[z]) && (y[0] == y[z])) {
            inGame = false;
        }
    }

    if (y[0] >= B_HEIGHT) {
        inGame = false;
    }

    if (y[0] < 0) {
        inGame = false;
    }

    if (x[0] >= B_WIDTH) {
        inGame = false;
    }

    if (x[0] < 0) {
        inGame = false;
    }

    if(!inGame) {
        killTimer(timerId);
    }
}

/* create new applle */
void Snake::locateApple() {

    QTime time = QTime::currentTime();
    qsrand((uint) time.msec());

    int r = qrand() % RAND_POS;
    apple_x = (r * DOT_SIZE);

    r = qrand() % RAND_POS;
    apple_y = (r * DOT_SIZE);
}

/* every timer tick, move snake (and repaint apple) */
void Snake::timerEvent(QTimerEvent *e) {

    Q_UNUSED(e);

    if (inGame) {

        checkApple();
        checkCollision();
        move();
    }

    repaint();
}

/* checks controls with controls saved in parent widget -> controls select */
void Snake::keyPressEvent(QKeyEvent *e) {

    QString key = e->text();

    qDebug() << key[0].unicode() << tolower(((ControlsSelect*)(parent()))->controls[0][1]);
    if (e->key() == Qt::Key_M) {
        if (((MainWindow*)(parent()->parent()->parent()))->music->isMuted())
            ((MainWindow*)(parent()->parent()->parent()))->music->setMuted(false);
        else
            ((MainWindow*)(parent()->parent()->parent()))->music->setMuted(true);
    }
    /* turn left */
    if (key[0].unicode() == tolower(((ControlsSelect*)(parent()))->controls[0][0])) {
        if (upDirection) {
            upDirection = false;
            leftDirection = true;
        }
        else if (leftDirection) {
            leftDirection = false;
            downDirection = true;
        }
        else if (downDirection) {
            downDirection = false;
            rightDirection = true;
        }
        else if (rightDirection) {
            rightDirection = false;
            upDirection = true;
        }
    }
    /* turn right */
    if (key[0].unicode() == tolower(((ControlsSelect*)(parent()))->controls[0][1])) {
        if (upDirection) {
            upDirection = false;
            rightDirection = true;
        }
        else if (rightDirection) {
            rightDirection = false;
            downDirection = true;
        }
        else if (downDirection) {
            downDirection = false;
            leftDirection = true;
        }
        else if (leftDirection) {
            leftDirection = false;
            upDirection = true;
        }
    }

    QWidget::keyPressEvent(e);
}

