cmake_minimum_required(VERSION 3.0)
project(player)

set(CMAKE_CXX_STANDARD 17)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)

# Find the Qt library
find_package(PkgConfig REQUIRED)    
find_package(Qt5Widgets CONFIG COMPONENTS Core Widgets Gui REQUIRED)


pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

qt_wrap_ui(UI_FILES mainwindow.ui)
qt_wrap_cpp(MOC_FILES libs/player.h 
                      libs/widgets/directoryViewList.h
                      libs/widgets/toggleButtonWidget.h
                      libs/widgets/playlistWidgetItem.h
                      libs/wiggets/directoryViewWidget.h
)
qt_add_resources(RESOURCES icons.qrc)

include_directories(libs build src/widgets libs/widgets ${GSTREAMER_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})

set(SRCS 
    src/main.cpp
    src/player.hpp
    src/effects.hpp
    src/widgets/directoryViewList.hpp
    src/widgets/toggleButtonWidget.hpp
    src/widgets/playlistWidgetItem.hpp
    src/widgets/presetDialogWindow.hpp
    src/widgets/directoryViewWidget.hpp
    src/widgets/directoryViewWidgetTree.hpp
)

set(HEADERS
    libs/player.h
    libs/effects.h
    libs/metaTypes.h
    libs/widgets/directoryViewList.h
    libs/widgets/toggleButtonWidget.h
    libs/widgets/playlistWidgetItem.h
    libs/widgets/presetDialogWindow.h
    libs/widgets/directoryViewWidget.h
    libs/widgets/directoryViewWidgetTree.h
    libs/widgets/directoryViewElementWidget.h
)

# Tell CMake to create the helloworld executable
add_executable(player ${SRCS} ${HEADERS} ${UI_FILES} ${MOC_FILES} ${RESOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(player Qt5::Widgets ${GSTREAMER_LIBRARIES} ${YAML_CPP_LIBRARIES} ${TAGLIB_LIBRARIES})

# Install the executable
install(TARGETS player DESTINATION bin)
