
#ifndef PRESET_DIALOG_WINDOW_H
#define PRESET_DIALOG_WINDOW_H


#include <string>


#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>


class PresetDialogWindow : public QDialog {
    Q_OBJECT
    
private:
    QLabel           dialogNameLabel;
    QVBoxLayout      *dialogVLayout;
    QHBoxLayout      *dialogNameLayout,
                     *dialogButtonLayout;
    
public:
    QLineEdit        *dialogLineEdit;
    QDialogButtonBox *dialogButtonBox;
    
    PresetDialogWindow (QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~PresetDialogWindow ();
    
    void setup ();
    
    std::string getLineInput ();
};

#endif
