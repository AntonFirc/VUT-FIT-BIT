#include "gamecontroller.h"
#include <QThread>

GameController::GameController()
{
    QObject();
}

void GameController::run(QRect *rect, int end) {
    while (true) {
        if (end)
            break;
        QThread().sleep(1);
        rect->setX(rect->x() - 100);
        rect->setY(rect->y() - 100);
    }
}
