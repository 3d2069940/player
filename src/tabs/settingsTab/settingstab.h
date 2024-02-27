
#ifndef _SETTINGSTAB_H_
#define _SETTINGSTAB_H_


#include "ui_settingstab.h"


class SettingsTab : public QWidget {

public:
    explicit SettingsTab (QWidget *parent=nullptr);
    virtual ~SettingsTab ();

private:
    Ui::SettingsTabWidget ui;
};


#endif // _SETTINGSTAB_H_
