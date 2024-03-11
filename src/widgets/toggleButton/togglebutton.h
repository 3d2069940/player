
#ifndef _TOGGLEBUTTON_H_
#define _TOGGLEBUTTON_H_

#include <QPushButton>


class ToggleButton : public QPushButton {

public:
    explicit ToggleButton (QWidget *parent = nullptr);
    virtual ~ToggleButton ();

//  setters
    void setLabels (const QStringList&  _labels);
    void setIcons  (const QList<QIcon>& _icons);
    void setState  (int);
//  getters
    int getState () const;

private:
    QList<QIcon> icons;
    QStringList labels;

    int stateId {0};
    bool iconsSet;

    void onButtonClicked ();
};

#endif // _TOGGLEBUTTON_H_
