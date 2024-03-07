
#ifndef _VISUALIZINGTAB_H_
#define _VISUALIZINGTAB_H_


#include <QBrush>
#include <QTimer>
#include <QLinearGradient>


#include "../tab.h"
#include "src/effects/effects.h"
#include "ui_visualizingtab.h"


class Effects;
class VisualizingTab : public QWidget, public ITab {
    Q_OBJECT
public:
    explicit VisualizingTab (QWidget *parent=nullptr);
    virtual ~VisualizingTab ();

//  setters
    void setEffects (Effects *_effects);
private:
    Ui::VisualizationTabWidget ui;

    QCPBars *bars;
    QLinearGradient gradient;

    Effects *effects;

    QTimer plotTimer;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

private slots:
    void onBandValueChanged      (int value);
    void onRateValueChanged      (int value);
    void onThresholdValueChanged (int value);

    void updatePlot ();
};

#endif // _VISUALIZINGTAB_H_
