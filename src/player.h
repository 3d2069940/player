#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui {
class player;
}

class player : public QMainWindow
{
    Q_OBJECT

public:
    explicit player(QWidget *parent = nullptr);
    ~player() override;

private:
    QScopedPointer<Ui::player> m_ui;
};

#endif // PLAYER_H
