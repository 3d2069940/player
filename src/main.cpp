#include "player.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    player w;
    w.show();

    return app.exec();
}

