
#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include <QList>
#include <QPushButton>

class QWidget;
class QMouseEvent;

class ToggleButton : public QPushButton {
    Q_OBJECT
    
public:
    explicit ToggleButton (QWidget *parent = nullptr);
    ~ToggleButton ();
    
    void setStates  (QList<QIcon> states);
    void setStateId (int);
    
    int stateId = 0;
    
private:
    QList<QIcon> statesList;
    
protected:
    void mousePressEvent (QMouseEvent *event) override;
};

#endif // TOGGLE_BUTTON_H

