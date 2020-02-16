#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H
#include <QRect>
#include <QObject>

class GameController : public QObject {
    Q_OBJECT

public:
    GameController();
    void run(QRect *rect, int end);
};

#endif // GAMECONTROLLER_H
