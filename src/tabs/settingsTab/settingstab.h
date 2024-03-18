
#ifndef _SETTINGSTAB_H_
#define _SETTINGSTAB_H_


#include "src/tabs/tab.h"
#include "ui_settingstab.h"


class SettingsTab : public QWidget, public ITab {

public:
    explicit SettingsTab (QWidget *parent=nullptr);

private:
    Ui::SettingsTabWidget ui;

    void createWidgets  () override;
    void setupWidgets   () override;
    void connectWidgets () override;

private slots:
    void onSelectFoldersButtonClicked ();
};


#endif // _SETTINGSTAB_H_
