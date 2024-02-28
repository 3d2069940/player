
#ifndef _VISUALIZINGTAB_H_
#define _VISUALIZINGTAB_H_


#include <QLinearGradient>
#include <qbrush.h>


#include "../tab.h"
#include "ui_visualizingtab.h"


class VisualizingTab : public QWidget, public ITab {

public:
    explicit VisualizingTab (QWidget *parent=nullptr);
    virtual ~VisualizingTab ();

private:
    Ui::VisualizationTabWidget ui;

    QCPBars *bars;
    QLinearGradient gradient;

    void inline createWidgets  () override;
    void inline setupWidgets   () override;
    void inline connectWidgets () override;
};

#endif // _VISUALIZINGTAB_H_
