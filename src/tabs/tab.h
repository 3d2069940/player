
#ifndef _TAB_H_
#define _TAB_H_

class ITab {

    virtual void createWidgets  () = 0;
    virtual void setupWidgets   () = 0;
    virtual void connectWidgets () = 0;
};

#endif // _TAB_H_
