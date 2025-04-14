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
#include <QLineEdit>
#include <QCheckBox>
#include <QPainter>
#include <QtMath>  // for exp(), M_PI
#include <QDebug>

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
    QPushButton *deleteChildBtn = new QPushButton("Delete Child", this);
    toolBar->addWidget(deleteChildBtn);
    connect(deleteChildBtn, &QPushButton::clicked, this, &MainWindow::handleDeleteChild);
    connect(btn1, &QPushButton::clicked, this, [this]()
            {
    // Get the currently selected node
    Node* selectedNode;
    CanvasWidget *canvas = findChild<CanvasWidget *>();
    if (!canvas) {
        QMessageBox::warning(this, "Error", "Canvas not found!");
        return;
    }
    
    selectedNode = canvas->getSelectedNode();
    if (!selectedNode) {
        QMessageBox::warning(this, "Error", "No node selected!");
        return;
    }
    
    // Get all available nodes
    QList<Node*> allNodes = canvas->getAllNodes();
    
    // Create a dialog for selecting the node to connect to
    QDialog dialog(this);
    dialog.setWindowTitle("Select Node to Connect");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // Add a list widget to show available nodes
    QListWidget *nodeListWidget = new QListWidget(&dialog);
    layout->addWidget(new QLabel("Select a node to connect to:"));
    layout->addWidget(nodeListWidget);
    
    // Populate the list with nodes that aren't the selected node
    for (Node *node : allNodes) {
        if (node != selectedNode) {
            // Create an item with node name and type
            QListWidgetItem *item = new QListWidgetItem(
                node->getName() + " (" + node->getType() + ")",
                nodeListWidget
            );
            // Store the node pointer as item data
            item->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(node)));
        }
    }
    
    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        &dialog
    );
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    // Show the dialog and process result
    if (dialog.exec() == QDialog::Accepted) {
        QListWidgetItem *selectedItem = nodeListWidget->currentItem();
        if (selectedItem) {
            // Get the node pointer from the item data
            Node *targetNode = reinterpret_cast<Node*>(
                selectedItem->data(Qt::UserRole).value<quintptr>()
            );
            
            // Connect the nodes
            selectedNode->addChildNode(targetNode);
            canvas->update();  // Redraw the canvas to show the connection
        }
    } });

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
    nodeList->addItem("Output");
    nodeList->setMaximumWidth(180);

    // When a list item is clicked, we create a node using the appropriate method.
    connect(nodeList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
    {
        CanvasWidget *canvas = findChild<CanvasWidget *>();
        if (!canvas) return;

        if (item->text() == "Load Image") {
            QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
            if (!fileName.isEmpty()) {
                QImage image;
                if (image.load(fileName)) {
                    canvas->loadImage(image, fileName);
                } else {
                    QMessageBox::warning(this, "Load Image", "Failed to load the image.");
                }
            }
        } else {
            // For all other node types, create a new node.
            // For example, when "Blur" is selected, a new Blur node is created.
            canvas->createNode(item->text());
        }
    });

    // === Canvas in Center ===
    CanvasWidget *canvas = new CanvasWidget();
    canvas->setMinimumSize(2000, 2000);
    // Connect the nodeSelected signal so that when a node is clicked or dragged, adjustments update.
    connect(canvas, &CanvasWidget::nodeSelected, this, &MainWindow::handleNodeSelected);
    QScrollArea *canvasScroll = new QScrollArea();
    canvasScroll->setWidget(canvas);
    canvasScroll->setWidgetResizable(true);
    canvasScroll->setMinimumSize(600, 600);

    // === Right Panel: Adjustments ===
    QFrame *adjustments = new QFrame();
    adjustments->setObjectName("adjustmentsPanel"); // Important!
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

MainWindow::~MainWindow()
{
}

// ---------------------------------------------------------------------
// Node selection and adjustment panel methods

void MainWindow::handleNodeSelected(Node *node)
{
    m_selectedNode = node;
    if (node)
        setupAdjustmentPanel(node);
    else
        clearAdjustmentPanel();
}

void MainWindow::updateNodeProperty(const QString &propertyName, const QVariant &value)
{
    if (m_selectedNode && m_selectedNode->hasProperty(propertyName)) {
        NodeProperty *prop = m_selectedNode->getProperty(propertyName);
        if (prop) {
            prop->setValue(value);
            // Optionally, trigger processing based on property changes here.
        }
    }
}

void MainWindow::setupAdjustmentPanel(Node *node)
{
    if (!node)
        return;

    QWidget *adjustmentsPanel = findChild<QWidget *>("adjustmentsPanel");
    if (!adjustmentsPanel)
        return;

    m_adjustmentsPanel = adjustmentsPanel;

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(adjustmentsPanel->layout());
    if (!mainLayout)
    {
        mainLayout = new QVBoxLayout(adjustmentsPanel);
        adjustmentsPanel->setLayout(mainLayout);
    }

    // Clear old scroll area if any
    QLayoutItem *item;
    while ((item = mainLayout->takeAt(0)) != nullptr)
    {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    // === Scroll Area Setup ===
    QScrollArea *scrollArea = new QScrollArea(adjustmentsPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);

    mainLayout->addWidget(scrollArea); // Add the scroll area to the panel layout

    // === Node Control Panel ===
    QLabel *titleLabel = new QLabel(node->getName() + " (" + node->getType() + ")");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    scrollLayout->addWidget(titleLabel);

    QList<NodeProperty *> properties = node->getAllProperties();
    for (NodeProperty *prop : properties)
    {
        QLabel *propLabel = new QLabel(prop->getName() + ":");
        scrollLayout->addWidget(propLabel);

        switch (prop->getType())
        {
            case NodeProperty::CustomList:
            {
                //this contains the QList of child nodes only for the load images
                QList<Node *> childNodes = prop->getValue().value<QList<Node *>>();
                QComboBox *comboBox = new QComboBox();
                for (Node *childNode : childNodes)
                {
                    comboBox->addItem(childNode->getName());
                }
                comboBox->setCurrentText(prop->getValue().toString());
                scrollLayout->addWidget(comboBox);
                connect(comboBox, &QComboBox::currentTextChanged, this, [this, prop](const QString &text)
                        { updateNodeProperty(prop->getName(), text); });
                break;

            }
        case NodeProperty::Blur_Radius:
        {
            // Implement Gaussian blur with configurable radius (1-20px)
            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setRange(1, 20);
            slider->setValue(prop->getValue().toInt());
            QLabel *valueLabel = new QLabel(QString::number(prop->getValue().toInt()));
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            sliderLayout->addWidget(slider);
            sliderLayout->addWidget(valueLabel);
            QWidget *container = new QWidget();
            container->setLayout(sliderLayout);
            scrollLayout->addWidget(container);
            //add a kernel matrix based on tthe slider value
            //connect the slider to the node property

            connect(slider, &QSlider::valueChanged, this, [this, valueLabel, prop](int value)
                    {
                valueLabel->setText(QString::number(value));
                updateNodeProperty(prop->getName(), value); });
            break;
                    
        }
        case NodeProperty::Image_contrast:{
            // Adjust image contrast with a slider (0 to 3)
            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setRange(0, 300);
            slider->setValue(static_cast<int>(prop->getValue().toDouble() * 100));
            QLabel *valueLabel = new QLabel(QString::number(prop->getValue().toDouble(), 'f', 2));
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            sliderLayout->addWidget(slider);
            sliderLayout->addWidget(valueLabel);
            QWidget *container = new QWidget();
            container->setLayout(sliderLayout);
            scrollLayout->addWidget(container);
            connect(slider, &QSlider::valueChanged, this, [this, valueLabel, prop](int value)
                    {
                double dValue = value / 100.0;
                valueLabel->setText(QString::number(dValue, 'f', 2));
                updateNodeProperty(prop->getName(), dValue); });
            break;
        } 
        case NodeProperty::Integer:
        {
            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setRange(-100, 100);
            slider->setValue(prop->getValue().toInt());

            QLabel *valueLabel = new QLabel(QString::number(prop->getValue().toInt()));
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            sliderLayout->addWidget(slider);
            sliderLayout->addWidget(valueLabel);
            QWidget *container = new QWidget();
            container->setLayout(sliderLayout);
            scrollLayout->addWidget(container);

            connect(slider, &QSlider::valueChanged, this, [this, valueLabel, prop](int value)
                    {
                valueLabel->setText(QString::number(value));
                updateNodeProperty(prop->getName(), value); });
            

            break;
        }
        case NodeProperty::Double:
        {
            QSlider *slider = new QSlider(Qt::Horizontal);
            slider->setRange(0, 100);
            slider->setValue(static_cast<int>(prop->getValue().toDouble() * 100));

            QLabel *valueLabel = new QLabel(QString::number(prop->getValue().toDouble(), 'f', 2));
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            sliderLayout->addWidget(slider);
            sliderLayout->addWidget(valueLabel);
            QWidget *container = new QWidget();
            container->setLayout(sliderLayout);
            scrollLayout->addWidget(container);

            connect(slider, &QSlider::valueChanged, this, [this, valueLabel, prop](int value)
                    {
                double dValue = value / 100.0;
                valueLabel->setText(QString::number(dValue, 'f', 2));
                updateNodeProperty(prop->getName(), dValue); });
            break;
        }
        case NodeProperty::String:
        {
            QLineEdit *lineEdit = new QLineEdit(prop->getValue().toString());
            scrollLayout->addWidget(lineEdit);
            connect(lineEdit, &QLineEdit::editingFinished, this, [this, lineEdit, prop]()
                    { updateNodeProperty(prop->getName(), lineEdit->text()); });
            break;
        }
        case NodeProperty::Boolean:
        {
            QCheckBox *checkBox = new QCheckBox("Enabled");
            checkBox->setChecked(prop->getValue().toBool());
            scrollLayout->addWidget(checkBox);
            connect(checkBox, &QCheckBox::toggled, this, [this, prop](bool checked)
                    { updateNodeProperty(prop->getName(), checked); });
            break;
        }
        case NodeProperty::Enum:
        {
            QComboBox *comboBox = new QComboBox();
            comboBox->addItems(prop->getEnumValues());
            comboBox->setCurrentText(prop->getValue().toString());
            scrollLayout->addWidget(comboBox);
            connect(comboBox, &QComboBox::currentTextChanged, this, [this, prop](const QString &text)
                    { updateNodeProperty(prop->getName(), text); });
            break;
        }
        }
    }
    if (node->getType() == "Output")
    {
        // Add a preview image widget
        QLabel *previewLabel = new QLabel("Preview:");
        scrollLayout->addWidget(previewLabel);

        QLabel *imagePreview = new QLabel();
        imagePreview->setMinimumSize(300, 200);
        imagePreview->setAlignment(Qt::AlignCenter);
        imagePreview->setStyleSheet("background-color: #333333; border: 1px solid #555555;");
        scrollLayout->addWidget(imagePreview);

        // Add a refresh preview button
        QPushButton *refreshButton = new QPushButton("Refresh Preview");
        scrollLayout->addWidget(refreshButton);
        connect(refreshButton, &QPushButton::clicked, this, [this, node, imagePreview]()
                {
            CanvasWidget *canvas = findChild<CanvasWidget *>();
            if (!canvas) return;
            
            QImage previewImage = canvas->processNodeGraph(node);
            if (!previewImage.isNull()) {
                // Scale down for preview if needed
                QImage scaledImage = previewImage;
                if (previewImage.width() > imagePreview->width() || 
                    previewImage.height() > imagePreview->height()) {
                    scaledImage = previewImage.scaled(
                        imagePreview->width(), 
                        imagePreview->height(),
                        Qt::KeepAspectRatio, 
                        Qt::SmoothTransformation
                    );
                }
                imagePreview->setPixmap(QPixmap::fromImage(scaledImage));
            } else {
                imagePreview->setText("No preview available");
            } });

        // Add a save button
        QPushButton *saveButton = new QPushButton("Save Image...");
        scrollLayout->addWidget(saveButton);
        connect(saveButton, &QPushButton::clicked, this, [this, node]()
                {
            QString format = "PNG";
            if (node->hasProperty("outputFormat")) {
                format = node->getProperty("outputFormat")->getValue().toString();
            }
            
            QString fileName = QFileDialog::getSaveFileName(
                this, 
                "Save Image",
                "",
                format + " Files (*." + format.toLower() + ")"
            );
            
            if (!fileName.isEmpty()) {
                CanvasWidget *canvas = findChild<CanvasWidget *>();
                if (canvas) {
                    canvas->saveOutputImage(node, fileName);
                }
            } });
    }
    scrollLayout->addStretch(); // Make sure content scrolls properly
}

void MainWindow::clearAdjustmentPanel()
{
    QWidget *adjustmentsPanel = findChild<QWidget *>("adjustmentsPanel");
    if (!adjustmentsPanel)
        return;

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(adjustmentsPanel->layout());
    if (!layout) {
        layout = new QVBoxLayout(adjustmentsPanel);
        adjustmentsPanel->setLayout(layout);
    }

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    QLabel *noSelectionLabel = new QLabel();
    noSelectionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(noSelectionLabel);
    layout->addStretch();
}

// ---------------------------------------------------------------------
// This method is used to connect the node list to the canvas node creation.
void MainWindow::setupNodeList()
{
    QListWidget *nodeList = findChild<QListWidget *>();
    if (!nodeList)
        return;

    connect(nodeList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
    {
        CanvasWidget *canvas = findChild<CanvasWidget *>();
        if (!canvas) return;

        if (item->text() == "Load Image") {
            QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.bmp)");
            if (!fileName.isEmpty()) {
                QImage image;
                if (image.load(fileName)) {
                    canvas->loadImage(image, fileName);
                } else {
                    // QMessageBox::warning(this, "Load Image", "Failed to load the image.");
                }
            }
        } else {
            // Create a node for other types (Blur, Sharpen, etc.)
            canvas->createNode(item->text());
        }
    });
}

void MainWindow::handleDeleteChild()
{
    // Get the currently selected node
    Node *selectedNode = nullptr;
    CanvasWidget *canvas = findChild<CanvasWidget *>();
    if (canvas)
    {
        selectedNode = canvas->getSelectedNode();
    }

    if (!selectedNode)
    {
        QMessageBox::warning(this, "Error", "No node selected!");
        return;
    }

    // If node has no children, show a message
    if (selectedNode->getChildren().isEmpty())
    {
        QMessageBox::information(this, "Info", "Selected node has no child nodes to delete.");
        return;
    }

    // Create a dialog to select which child to delete
    QDialog dialog(this);
    dialog.setWindowTitle("Select Child to Delete");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QComboBox *childSelector = new QComboBox(&dialog);
    for (Node *child : selectedNode->getChildren())
    {
        childSelector->addItem(child->getName() + " (" + child->getType() + ")", QVariant::fromValue(reinterpret_cast<quintptr>(child)));
    }

    layout->addWidget(new QLabel("Select a child node to delete:"));
    layout->addWidget(childSelector);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    // Execute the dialog
    if (dialog.exec() == QDialog::Accepted)
    {
        int selectedIndex = childSelector->currentIndex();
        if (selectedIndex >= 0 && selectedIndex < selectedNode->getChildren().size())
        {
            // Get the selected child node
            Node *childToRemove = selectedNode->getChildren().at(selectedIndex);

            // Remove the child node from the parent
            selectedNode->removeChildNode(childToRemove);

            // Update the canvas
            canvas->update();
        }
    }
}
