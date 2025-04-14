cmake_minimum_required(VERSION 3.16)
project(NodeImageEditor)

set(CMAKE_CXX_STANDARD 17)

# Automatically handle MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the necessary Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

# Find OpenCV package
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executable and include necessary source files
add_executable(NodeImageEditor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    canvaswidget.cpp
    canvaswidget.h
    node.cpp
    node.h
    node_property.h
    image_processor.cpp
    image_processor.h
)

# Link the necessary Qt6 libraries and OpenCV
target_link_libraries(NodeImageEditor PRIVATE 
    Qt6::Widgets 
    Qt6::Core
    ${OpenCV_LIBS}
)

# Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)
