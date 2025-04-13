#include "mainwindow.h"
#include "canvaswidget.h"
#include <QFileDialog>
#include <QImage>
#include <QMenuBar>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QComboBox>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Node Image Editor");
    resize(1200, 800);

    // === Menu Bar ===
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu("File", this);
    fileMenu->addAction("New");
    fileMenu->addAction("Open");
    fileMenu->addAction("Save");
    fileMenu->addAction("Save As");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");
    menuBar->addMenu(fileMenu);
    QMenu *editMenu = new QMenu("Edit", this);
    editMenu->addAction("Undo");
    editMenu->addAction("Redo");
    editMenu->addSeparator();
    editMenu->addAction("Cut");
    editMenu->addAction("Copy");
    editMenu->addAction("Paste");
    menuBar->addMenu(editMenu);
    QMenu *viewMenu = new QMenu("View", this);
    viewMenu->addAction("Zoom In");
    viewMenu->addAction("Zoom Out");
    viewMenu->addAction("Reset Zoom");
    menuBar->addMenu(viewMenu);
    QMenu *drawChildrenMenu = new QMenu("Draw Children", this);
    drawChildrenMenu->addAction("Draw Children");
    menuBar->addMenu(drawChildrenMenu);
    setMenuBar(menuBar);

    // === ToolBar ===
    QToolBar *toolBar = new QToolBar("Main Toolbar", this);
    toolBar->setMovable(false);
    QMenu *fileMenu2 = new QMenu("File", this);
    fileMenu2->addAction("New");
    fileMenu2->addAction("Open");
    fileMenu2->addAction("Save");
    fileMenu2->addAction("Save As");
    fileMenu2->addSeparator();
    fileMenu2->addAction("Exit");
    QMenu *editMenu2 = new QMenu("Edit", this);
    editMenu2->addAction("Undo");
    editMenu2->addAction("Redo");
    editMenu2->addSeparator();
    editMenu2->addAction("Cut");
    editMenu2->addAction("Copy");
    editMenu2->addAction("Paste");
    QMenu *viewMenu2 = new QMenu("View", this);
    viewMenu2->addAction("Zoom In");
    viewMenu2->addAction("Zoom Out");
    viewMenu2->addAction("Reset Zoom");
    QPushButton *btn1 = new QPushButton("Draw Children", this);
    toolBar->addAction(fileMenu2->menuAction());
    toolBar->addAction(editMenu2->menuAction());
    toolBar->addAction(viewMenu2->menuAction());
    toolBar->addWidget(btn1);
    connect(btn1, &QPushButton::clicked, this, [this]()
            { QMessageBox::information(this, "Draw Children", "Draw Children button clicked!"); });
    addToolBar(Qt::TopToolBarArea, toolBar);

    // === Central Layout Setup ===
    QWidget *mainArea = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainArea);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // === Left Panel: Node / Function List ===
    QListWidget *nodeList = new QListWidget();
    nodeList->addItem("Load Image");
    nodeList->addItem("Blur");
    nodeList->addItem("Sharpen");
    nodeList->addItem("Grayscale");
    nodeList->addItem("Brightness");
    nodeList->setMaximumWidth(180);

    connect(nodeList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
            {
    if (item->text() == "Load Image") {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
        if (!fileName.isEmpty()) {
            QImage image;
            if (image.load(fileName)) {
                CanvasWidget *canvas = findChild<CanvasWidget *>();
                canvas->loadImage(image);
            } else {
                QMessageBox::warning(this, "Load Image", "Failed to load the image.");
            }
        }
    } else if (item->text() == "Blur") {
        QImage blurImage(1000, 750, QImage::Format_ARGB32_Premultiplied);
        blurImage.fill(Qt::white);
        QPainter painter(&blurImage);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 48));
        painter.drawText(blurImage.rect(), Qt::AlignCenter, "Blur");

        CanvasWidget *canvas = findChild<CanvasWidget *>();
        if (canvas) {
            canvas->loadImage(blurImage);
        }

        QWidget *adjustmentsPanel = findChild<QWidget *>("adjustmentsPanel");
        if (adjustmentsPanel) {
            QLayout *layout = adjustmentsPanel->layout();
            if (!layout) {
                layout = new QVBoxLayout(adjustmentsPanel);
                adjustmentsPanel->setLayout(layout);
            }

            QLayoutItem *item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }

            QLabel *radiusLabel = new QLabel("Blur Radius:");
            QSlider *radiusSlider = new QSlider(Qt::Horizontal);
            radiusSlider->setRange(1, 20);
            radiusSlider->setValue(5);

            QLabel *typeLabel = new QLabel("Blur Type:");
            QComboBox *blurTypeCombo = new QComboBox();
            blurTypeCombo->addItem("Uniform");
            blurTypeCombo->addItem("Directional");

            QLabel *kernelPreviewLabel = new QLabel("Kernel Preview:");
            QLabel *kernelMatrix = new QLabel();
            kernelMatrix->setStyleSheet("QLabel { font-family: monospace; }");
            kernelMatrix->setTextInteractionFlags(Qt::TextSelectableByMouse);

            auto updateKernelPreview = [=]() {
                int r = radiusSlider->value();
                QString blurType = blurTypeCombo->currentText();
                QString matrix;
                int size = r * 2 + 1;

                if (blurType == "Uniform") {
                    float val = 1.0f / (size * size);
                    for (int i = 0; i < size; ++i) {
                        for (int j = 0; j < size; ++j)
                            matrix += QString("%1 ").arg(val, 0, 'f', 2);
                        matrix += "\n";
                    }
                } else {
                    float val = 1.0f / size;
                    for (int i = 0; i < size; ++i) {
                        for (int j = 0; j < size; ++j)
                            matrix += (j == size / 2) ? QString("%1 ").arg(val, 0, 'f', 2) : "0.00 ";
                        matrix += "\n";
                    }
                }

                kernelMatrix->setText(matrix);
kernelMatrix->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
kernelMatrix->setWordWrap(true);
kernelMatrix->setMaximumHeight(150);  // Limit height
kernelMatrix->setMinimumWidth(150);  
            };

            connect(radiusSlider, &QSlider::valueChanged, this, [=](int) {
                updateKernelPreview();
            });
            connect(blurTypeCombo, &QComboBox::currentTextChanged, this, [=](const QString &) {
                updateKernelPreview();
            });

            updateKernelPreview();

            layout->addWidget(radiusLabel);
            layout->addWidget(radiusSlider);
            layout->addWidget(typeLabel);
            layout->addWidget(blurTypeCombo);
            layout->addWidget(kernelPreviewLabel);
            layout->addWidget(kernelMatrix);
        }
    } else if (item->text() == "Sharpen") {
        QMessageBox::information(this, "Sharpen", "Sharpen function selected!");
    } else if (item->text() == "Grayscale") {
        QMessageBox::information(this, "Grayscale", "Grayscale function selected!");
    } else if (item->text() == "Brightness") {
        QMessageBox::information(this, "Brightness", "Brightness function selected!");
    } });

    // === Canvas in Center ===
    CanvasWidget *canvas = new CanvasWidget();
    canvas->setMinimumSize(2000, 2000);
    QScrollArea *canvasScroll = new QScrollArea();
    canvasScroll->setWidget(canvas);
    canvasScroll->setWidgetResizable(true);
    canvasScroll->setMinimumSize(600, 600);

    // === Right Panel: Adjustments ===
    QFrame *adjustments = new QFrame();
    adjustments->setObjectName("adjustmentsPanel"); // <=== important!
    adjustments->setFrameShape(QFrame::StyledPanel);
    adjustments->setMinimumWidth(180);
    QVBoxLayout *adjustLayout = new QVBoxLayout(adjustments);
    QLabel *label = new QLabel("Adjustments");
    adjustLayout->addWidget(label);
    adjustLayout->addStretch();

    // === Add to Layout ===
    mainLayout->addWidget(nodeList);
    mainLayout->addWidget(canvasScroll, 1);
    mainLayout->addWidget(adjustments);

    setCentralWidget(mainArea);
}

MainWindow::~MainWindow() {}
