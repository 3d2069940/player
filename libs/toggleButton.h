
#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include <QList>
#include <QPair>
#include <QWidget>
#include <QPushButton>


class ToggleButton : public QPushButton {
    Q_OBJECT
    
public:
    ToggleButton (QList<QPair<QString, QIcon>> states = {});
    ~ToggleButton ();
    
private:
    QList<QPair<QString, QIcon>> statesList;
    int stateId;
    
protected:
    void mousePressEvent (QMouseEvent *event) override;
};

#endif // TOGGLE_BUTTON_WIDGET_H

