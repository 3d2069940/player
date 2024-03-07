
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_


#include <QMainWindow>


#include "effects/effects.h"
#include "parser/parser.h"
#include "db/db.h"


#include "ui_mainwindow.h"


class PlayerTab;
class PlaylistTab;
class EffectsTab;
class VisualizingTab;
class SettingsTab;

class MainWindowUI : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindowUI (QWidget *parent=nullptr);
    virtual ~MainWindowUI ();

private:
    Ui::MainWindow ui;

    QSharedPointer<PlayerTab>      playerTab;
    QSharedPointer<PlaylistTab>    playlistTab;
    QSharedPointer<EffectsTab>     effectsTab;
    QSharedPointer<VisualizingTab> visualizingTab;
    QSharedPointer<SettingsTab>    settingsTab;

    Effects effects;
    Parser  parser;
    DataBase db;

    void connectWidgets ();
};

#endif // _MAINWINDOW_H_
