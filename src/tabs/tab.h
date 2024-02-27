
#ifndef _TAB_H_
#define _TAB_H_

class Tab {

    virtual void createWidgets () = 0;
    virtual void setupTab      () = 0;
    virtual void connectTab    () = 0;
};

#endif // _TAB_H_
