
//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "player.hpp"


int main (int argc, char** argv) {
    QApplication app(argc, argv);
    gst_init(&argc,&argv);

    MainWindowUI ui;
    ui.show();
    
    return app.exec();
}
