
#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H


class ToggleButton : public QPushButton {

public:
    explicit ToggleButton (QWidget *parent = nullptr);
    virtual ~ToggleButton ();

    void setLabels (QStringList&&  _labels);
    void setIcons  (QList<QIcon>&& _icons);
    void setState  (int);

    int  getState () const;

private:
    QList<QIcon> icons;
    QStringList labels;

    int stateId {0};
    bool iconsSet;

private slots:
    void buttonClicked ();
};

#endif // TOGGLE_BUTTON_H
