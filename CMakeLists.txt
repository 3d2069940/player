
cmake_minimum_required(VERSION 3.0)

include(ProcessorCount)

project(player)

set(CMAKE_CXX_STANDARD 17)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)

ProcessorCount(CPU_NUM)

find_program(QMAKE qmake)
find_program(MAKE make)

find_package(PkgConfig REQUIRED)    
find_package(Qt5Widgets CONFIG COMPONENTS Core Widgets Gui REQUIRED)

pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(SNDFILE REQUIRED sndfile)

qt_wrap_ui(UI_FILES mainwindow.ui)
qt_wrap_cpp(MOC_FILES src/player.h 
                      src/widgets/player/playlistWidgetItem.h
                      src/widgets/effects/presetDialogWindow.h
)
qt_add_resources(RESOURCES icons.qrc)

include_directories(src build libs/qcustomplot
                    src/widgets/settings
                    src/widgets/player
                    src/widgets/buttons
                    ${GSTREAMER_INCLUDE_DIRS} 
                    ${YAML_CPP_INCLUDE_DIRS} 
                    ${TAGLIB_INCLUDE_DIRS}
                    ${SNDFILE_INCLUDE_DIRS}
)

link_directories(libs/qcustomplot src/widgets/settings)

set(SRCS
    src/main.cpp
    src/player.hpp
    src/effects.hpp
    src/connector.cpp
    src/widgets/buttons/toggleButton.hpp
    src/widgets/player/playlistWidgetItem.hpp
    src/widgets/effects/presetDialogWindow.hpp
    src/widgets/delegation/playlistDelegate.hpp
)

set(HEADERS
    src/player.h
    src/effects.h
    src/metaTypes.h
    src/widgets/buttons/toggleButton.h
    src/widgets/player/playlistWidgetItem.h
    src/widgets/effects/presetDialogWindow.h
)

add_custom_target(QCustomPlot
    COMMAND qmake && make -j${CPU_NUM}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/qcustomplot
    COMMENT "QCustomPlot compilation"
)

add_subdirectory(src/widgets/settings)
add_executable(player ${SRCS} ${HEADERS} ${UI_FILES} ${MOC_FILES} ${RESOURCES})
add_dependencies(player QCustomPlot DirectoryListView)


# Use the Widgets module from Qt 5.
target_link_libraries(player qcustomplot DirectoryListView
                            ${GSTREAMER_LIBRARIES}
                            ${YAML_CPP_LIBRARIES}
                            ${TAGLIB_LIBRARIES}
                            ${SNDFILE_LIBRARIES})

# Install the executable
install(TARGETS player DESTINATION bin)
