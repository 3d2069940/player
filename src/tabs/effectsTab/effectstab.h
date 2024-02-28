
#ifndef _EFFECTSTAB_H_
#define _EFFECTSTAB_H_

#include "../tab.h"

#include "src/effects/effects.h"
#include "ui_effectstab.h"


class Effects;
class EffectsTab : public QWidget, public ITab {

public:
    explicit EffectsTab (QWidget *parent=nullptr);
    virtual ~EffectsTab ();

    void setEffects (Effects *_effects);

private:
    Ui::EffectsTabWidget ui;

    Effects *effects;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;
      void connectEqualizer  ();
      void connectDelay      ();
      void connectFilter     ();
      void connectPitch      ();
      void connectCompressor ();
      void connectPanorama   ();

private slots:
    void onEqualizerValueChanged(CODES::EQUALIZER code, int    value);
    void onDelayValueChanged    (CODES::DELAY     code, int    value=0);
    void onFilterValueChanged   (CODES::FILTER    code, gfloat value=0);
};

#endif // _EFFECTSTAB_H_
