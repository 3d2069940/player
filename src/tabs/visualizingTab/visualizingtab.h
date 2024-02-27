
#ifndef _VISUALIZINGTAB_H_
#define _VISUALIZINGTAB_H_


#include <QLinearGradient>
#include <qbrush.h>


#include "../tab.h"
#include "ui_visualizingtab.h"


class VisualizingTab : public QWidget, public Tab {

public:
    explicit VisualizingTab (QWidget *parent=nullptr);
    virtual ~VisualizingTab ();

private:
    Ui::VisualizationTabWidget ui;

    QCPBars *bars;
    QLinearGradient gradient;

    void inline createWidgets () override;
    void inline setupTab      () override;
    void inline connectTab    () override;
};

#endif // _VISUALIZINGTAB_H_
