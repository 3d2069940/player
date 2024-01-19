TEMPLATE = app
TARGET = player

CONFIG += c++17 resources forms

QT += core widgets

AUTOMOC += libs/player.h \
		   libs/widgets/toggleButton.h \
           libs/widgets/playlistWidgetItem.h \
           libs/widgets/directoryViewWidget.h

INCLUDEPATH += src \
               libs \
               src/widgets \
               libs/widgets \
               /usr/include/taglib \
               /usr/include/glib-2.0 \
               /usr/include/gstreamer-1.0 \
               /usr/include/x86_64-linux-gnu \
               /usr/lib/x86_64-linux-gnu/glib-2.0/include

LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -pthread -lyaml-cpp -ltag

FORMS += forms/mainwindow.ui

RESOURCES += icons.qrc

SOURCES += src/main.cpp \
           src/player.hpp \
           src/effects.hpp \
           src/widgets/directoryViewList.hpp \
           src/widgets/toggleButtonWidget.hpp \
           src/widgets/playlistWidgetItem.hpp \
           src/widgets/presetDialogWindow.hpp \
           src/widgets/directoryViewWidget.hpp \
           src/widgets/directoryViewWidgetTree.hpp \
           src/widgets/directoryViewWidgetTree.hpp \
           src/widgets/directoryViewElementWidget.hpp

HEADERS += libs/player.h \
           libs/effects.h \
           libs/metaTypes.h \
           libs/widgets/directoryViewList.h \
           libs/widgets/toggleButtonWidget.h \
           libs/widgets/playlistWidgetItem.h \
           libs/widgets/presetDialogWindow.h \
           libs/widgets/directoryViewWidget.h \
           libs/widgets/directoryViewWidgetTree.h \
           libs/widgets/directoryViewElementWidget.h

MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
