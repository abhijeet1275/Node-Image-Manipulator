#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"
#include "node.h"
#include "node_property.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Optional: Set a clean modern style (Fusion is cross-platform)
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    window.setWindowTitle("Node-Based Image Editor");
    window.showMaximized(); // Launch in maximized mode for full view

    return app.exec();
}
