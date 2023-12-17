
//***********************************************************//
// Homebrew Classes
//***********************************************************//
#include "player.hpp"


int main (int argc, char** argv) {
    QApplication app(argc, argv);
        
    MainWindowUI ui;
    ui.show();
    
    return app.exec();
}

