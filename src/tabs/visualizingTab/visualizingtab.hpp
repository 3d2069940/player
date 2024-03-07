
#ifndef _VISUALIZINGTAB_HPP_
#define _VISUALIZINGTAB_HPP_

#include "src/effects/effects.h"
#include "visualizingtab.h"
#include <qspinbox.h>

VisualizingTab::VisualizingTab (QWidget *parent) 
    : QWidget (parent) {
    ui.setupUi(this);
    createWidgets  ();
    setupWidgets   ();
    connectWidgets ();

    plotTimer.start();
}

VisualizingTab::~VisualizingTab () {
    ui.visualizingCustomPlot->clearPlottables();
}

void VisualizingTab::createWidgets () {
    // seg fault with qsharedpointer
    bars  = new QCPBars(ui.visualizingCustomPlot->xAxis, ui.visualizingCustomPlot->yAxis);
}

void VisualizingTab::setupWidgets () {
    plotTimer.setInterval(75);

    ui.visualizingCustomPlot->xAxis->setVisible(false);
    ui.visualizingCustomPlot->yAxis->setVisible(false);

    ui.visualizingCustomPlot->axisRect()->setAutoMargins(QCP::msNone);
    ui.visualizingCustomPlot->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    ui.visualizingCustomPlot->yAxis->setRange(0, 120);
    ui.visualizingCustomPlot->xAxis->setRange(0, 25);
    
    ui.visualizingCustomPlot->setBackground(palette().color(QPalette::Window));

    gradient.setStart (width()/2, height()/2);
    gradient.setFinalStop(width()/2, height()-ui.visualizingCustomPlot->height());
      
    gradient.setColorAt(1, qApp->palette().color(QPalette::Link));
    gradient.setColorAt(0, qApp->palette().color(QPalette::Highlight));
    
    bars->setPen(Qt::NoPen);
    bars->setWidth(1);
    bars->setBrush(QBrush(gradient));
}

void VisualizingTab::connectWidgets () {
    connect(ui.visualizingBandsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualizingTab::onBandValueChanged);
    connect(ui.visualizingRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualizingTab::onRateValueChanged);
    connect(ui.visualizingThresholdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),this,&VisualizingTab::onThresholdValueChanged);

    connect(&plotTimer, &QTimer::timeout, this, &VisualizingTab::updatePlot);
}

void VisualizingTab::setEffects (Effects *_effects) {
    effects = _effects;
}

void VisualizingTab::onBandValueChanged (int value) {
    using namespace CODES;
    effects->changeSpectrumProps(SPECTRUM::Bands, value);
}

void VisualizingTab::onRateValueChanged (int value) {
    using namespace CODES;
    effects->changeSpectrumProps(SPECTRUM::UpdateRate, value);
    plotTimer.setInterval(value);
}

void VisualizingTab::onThresholdValueChanged (int value) {
    using namespace CODES;
    effects->changeSpectrumProps(SPECTRUM::AudioThreshold, value);
}

void VisualizingTab::updatePlot () {
    if (!effects->magnitudesChanged.load())
        return;

    QVector<double> dataX (effects->currentMagnitudes.size()), 
                    dataY (effects->currentMagnitudes.begin(), effects->currentMagnitudes.end());
    
    int thresholdValue = ui.visualizingThresholdSpinBox->value();
    
    for (int i = 0; i < dataX.size(); ++i) {
        dataY[i] -= thresholdValue;
        dataX[i] = i+0.5;
    }
    
    bars->setData(dataX, dataY, true);
    
    ui.visualizingCustomPlot->xAxis->setRange(0, effects->currentMagnitudes.size());
    ui.visualizingCustomPlot->replot();
    effects->magnitudesChanged.store(false);
}

#endif // _VISUALIZINGTAB_HPP_
