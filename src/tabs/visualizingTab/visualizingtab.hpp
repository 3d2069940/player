
#ifndef _VISUALIZINGTAB_HPP_
#define _VISUALIZINGTAB_HPP_

#include "visualizingtab.h"

VisualizingTab::VisualizingTab (QWidget *parent) 
    : QWidget (parent) {
    ui.setupUi(this);
    createWidgets();
    setupTab     ();
    connectTab   ();
}

VisualizingTab::~VisualizingTab () {
    ui.visualizingCustomPlot->clearPlottables();
}

void VisualizingTab::createWidgets () {
    // seg fault with qsharedpointer
    bars  = new QCPBars(ui.visualizingCustomPlot->xAxis, ui.visualizingCustomPlot->yAxis);
}

void VisualizingTab::setupTab () {
    ui.visualizingCustomPlot->xAxis->setVisible(false);
    ui.visualizingCustomPlot->yAxis->setVisible(false);

    ui.visualizingCustomPlot->axisRect()->setAutoMargins(QCP::msNone);
    ui.visualizingCustomPlot->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    ui.visualizingCustomPlot->yAxis->setRange(0, 120);
    ui.visualizingCustomPlot->xAxis->setRange(0, 10);
    
    ui.visualizingCustomPlot->setBackground(palette().color(QPalette::Window));

    gradient.setStart (width()/2, height()/2);
    gradient.setFinalStop(width()/2, height()-ui.visualizingCustomPlot->height());
      
    gradient.setColorAt(1, qApp->palette().color(QPalette::Window));
    gradient.setColorAt(0, qApp->palette().color(QPalette::Highlight));
    
    bars->setPen(Qt::NoPen);
    bars->setWidth(1);
    bars->setBrush(QBrush(gradient));
}

void VisualizingTab::connectTab () {

}

#endif // _VISUALIZINGTAB_HPP_
