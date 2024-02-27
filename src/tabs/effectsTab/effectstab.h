
#ifndef _EFFECTSTAB_H_
#define _EFFECTSTAB_H_

#include "ui_effectstab.h"

class EffectsTab : public QWidget {

public:
    explicit EffectsTab (QWidget *parent=nullptr);
    virtual ~EffectsTab ();

private:
    Ui::EffectsTabWidget ui;
};

#endif // _EFFECTSTAB_H_
