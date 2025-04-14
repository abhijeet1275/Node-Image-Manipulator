# Node-Based Image Editor

## Overview

This repository contains a Qt and OpenCV-based image editing application that uses a node-based architecture. Unlike traditional image editors, this application allows users to create a processing pipeline by connecting different types of nodes (blur, sharpen, grayscale, etc.) to achieve complex image manipulations in a visual and intuitive way.

<img width="1440" alt="Screenshot 2025-04-14 at 2 35 51 PM" src="https://github.com/user-attachments/assets/e1b7492e-cf7e-4935-b518-53d38402b1a5" />


## Features

- **Node-based workflow**: Create, connect, and arrange processing nodes on a canvas
- **Interactive UI**: Drag and drop nodes to organize your processing pipeline
- **Real-time adjustments**: Modify node parameters and see changes reflected in the output
- **Image processing operations**:
  - Load images from files
  - Apply blur effects (Uniform or Directional)
  - Adjust brightness and contrast
  - Convert to grayscale with different methods (Average, Luminosity, Lightness)
  - Apply sharpening with configurable amount
  - Save processed images

## Dependencies

- Qt 6
- OpenCV
- CMake (3.16 or later)
- C++17 compatible compiler

## Building the Project

### Prerequisites

Make sure you have the following installed:
- Qt 6 development libraries
- OpenCV
- CMake
- A modern C++ compiler

### Build Instructions

1. Clone this repository
   ```bash
   git clone https://github.com/username/NodeImageEditor.git
   cd NodeImageEditor
   ```

2. Create a build directory
   ```bash
   mkdir build
   cd build
   ```

3. Configure with CMake
   ```bash
   cmake ..
   ```

4. Build the project
   ```bash
   cmake --build .
   ```

5. Run the application
   ```bash
   ./NodeImageEditor
   ```

## Usage Guide

1. **Loading Images**:
   - Click on the "Load Image" item in the left panel
   - Select an image file from your file system

2. **Adding Processing Nodes**:
   - Click on a node type from the left panel (Blur, Sharpen, etc.)
   - The node will appear on the canvas

3. **Connecting Nodes**:
   - Select a node
   - Click "Draw Children" in the toolbar
   - Choose another node to connect them
   - Nodes are processed from parent to child

4. **Adjusting Node Parameters**:
   - Select a node on the canvas
   - Use the controls in the right panel to adjust parameters
   - Parameters are specific to each node type

5. **Previewing Results**:
   - Add an "Output" node
   - Connect it to your processing chain
   - Click "Refresh Preview" in the right panel

6. **Saving Results**:
   - Select the Output node
   - Click "Save Image" in the right panel
   - Choose a location and format to save the processed image

## Project Structure

- `main.cpp`: Application entry point
- `mainwindow.cpp/h`: Main application window and UI setup
- `canvaswidget.cpp/h`: The canvas where nodes are created and connected
- `node.cpp/h`: Node class implementation
- `node_property.h`: Property system for nodes
- `image_processor.cpp/h`: Image processing operations using OpenCV
