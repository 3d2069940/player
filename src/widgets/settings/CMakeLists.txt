cmake_minimum_required(VERSION 3.0)

project(DirectoryListView)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5Widgets CONFIG COMPONENTS Core Widgets Gui REQUIRED)

qt_wrap_cpp(MOC_WIDGET_FILES
    directoryListView.h
    directoryListWidget.h
)

set(DIRECTORY_LIST_VIEW_SRCS
    directoryListView.hpp
    directoryListWidget.hpp
    directoryTree.hpp
)

set(DIRECTORY_LIST_VIEW_HEADERS
    directoryListView.h
    directoryListWidget.h
    directoryTree.h
)

add_library(${PROJECT_NAME} STATIC ${DIRECTORY_LIST_VIEW_SRCS} ${DIRECTORY_LIST_VIEW_HEADERS} ${MOC_WIDGET_FILES})

target_compile_options(DirectoryListView PRIVATE "-Wall" "-Wextra")

target_link_libraries(DirectoryListView Qt5::Widgets Qt5::Core)
