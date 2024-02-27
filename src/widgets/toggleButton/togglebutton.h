
#ifndef _TOGGLEBUTTON_H_
#define _TOGGLEBUTTON_H_

#include <QPushButton>


class ToggleButton : public QPushButton {

public:
    explicit ToggleButton (QWidget *parent = nullptr);
    virtual ~ToggleButton ();

    void inline setLabels (const QStringList&  _labels);
    void inline setIcons  (const QList<QIcon>& _icons);


    void inline setState  (int);

    int  inline getState () const;

private:
    QList<QIcon> icons;
    QStringList labels;

    int stateId {0};
    bool iconsSet;

protected:
    void mousePressEvent (QMouseEvent *event) override;
};

#endif // _TOGGLEBUTTON_H_
