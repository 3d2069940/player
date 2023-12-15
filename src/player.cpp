#include "player.h"
#include "ui_player.h"

player::player(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::player)
{
    m_ui->setupUi(this);
}

player::~player() = default;
