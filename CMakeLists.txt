cmake_minimum_required(VERSION 2.8)

project(findString)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "mainwindow.h"
    "mainwindow.cpp"
    "mainwindow.ui"
    "indexator.cpp"
    "indexator.h"
    "finder.cpp"
    "finder.h"
    "trigram.h")
qt5_use_modules(${PROJECT_NAME} Widgets)
