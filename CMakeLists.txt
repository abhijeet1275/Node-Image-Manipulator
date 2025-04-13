cmake_minimum_required(VERSION 3.16)
project(NodeImageEditor)

set(CMAKE_CXX_STANDARD 17)

# Automatically handle MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the necessary Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# Add executable and include necessary source files
add_executable(NodeImageEditor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    canvaswidget.cpp
    canvaswidget.h
    node.cpp
    node.h
)

# Link the necessary Qt6 libraries
target_link_libraries(NodeImageEditor PRIVATE Qt6::Widgets Qt6::Core)

# Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)

