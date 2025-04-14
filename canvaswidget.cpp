// canvaswidget.cpp
#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: gray;");
    setMouseTracking(true); // Enable mouse tracking for the widget
}
void CanvasWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Optional: draw light grid
    painter.setPen(QPen(Qt::lightGray, 1));
    const int gridSize = 20;
    for (int x = 0; x < width(); x += gridSize)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += gridSize)
        painter.drawLine(0, y, width(), y);

    // Draw each node
    for (const Node &node : m_nodes)
    {
        // First draw the selection highlight if needed
        if (node.isSelected())
        {
            QRect rect(node.getPosition(), node.getImage().size());
            painter.setPen(QPen(Qt::yellow, 3));
            painter.drawRect(rect);
        }

        // Draw the node image
        painter.drawImage(node.getPosition(), node.getImage());

        // Draw the node name
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setBold(true);
        painter.setFont(font);
        QRect textRect(node.getPosition().x(),
                       node.getPosition().y() - 20,
                       node.getImage().width(),
                       20);
        painter.drawText(textRect, Qt::AlignCenter, node.getName());

        // Draw connections (if there are child nodes)
        if (!node.getChildren().isEmpty())
        {
            for (Node *childNode : node.getChildren())
            {
                QPoint start = node.getPosition() + QPoint(node.getImage().width() / 2, node.getImage().height() / 2);
                QPoint end = childNode->getPosition() + QPoint(childNode->getImage().width() / 2, childNode->getImage().height() / 2);
                painter.setPen(QPen(Qt::black, 2));
                painter.drawLine(start, end); // Draw line from parent to child
            }
        }
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    bool nodeClicked = false;

    // First deselect all nodes
    for (Node &node : m_nodes)
    {
        node.setSelected(false);
    }

    // Check if the mouse click is within any of the nodes
    for (int i = m_nodes.size() - 1; i >= 0; --i) // Check in reverse to handle overlapping nodes
    {
        Node &node = m_nodes[i];
        QRect nodeRect(node.getPosition(), node.getImage().size());
        if (nodeRect.contains(event->pos()))
        {
            // Select this node
            node.setSelected(true);

            // Set up dragging
            node.setDragging(true);
            m_draggedNode = &node; // Store the node being dragged
            m_offset = event->pos() - node.getPosition();

            // Emit the signal with the selected node
            emit nodeSelected(&node);

            nodeClicked = true;
            break;
        }
    }

    update(); // Request a repaint to show selection changes
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggedNode && m_draggedNode->isDragging())
    {
        // Update the position of the dragged node
        m_draggedNode->setPosition(event->pos() - m_offset);
        update(); // Request a repaint
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (m_draggedNode)
    {
        m_draggedNode->setDragging(false); // Stop dragging
        // Note: We don't reset the selected state here
        m_draggedNode = nullptr; // Clear the dragged node reference
    }
}

void CanvasWidget::loadImage(const QImage &image, const QString &filePath)
{
    // Position the node in the center of the canvas
    double scale_factor = 0.2;
    int scaledWidth = image.width() * scale_factor;
    int scaledHeight = image.height() * scale_factor;
    QImage scaledImage = image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Create a unique name for the node
    QString nodeName = "Image_" + QString::number(++m_nodeCounter);

    // Set the position to a fixed point for simplicity
    QPoint position(50, 50); // Initial position
    Node newNode(scaledImage, position, "Load Image", nodeName);

    // Set additional properties
    if (newNode.hasProperty("filePath"))
    {
        newNode.getProperty("filePath")->setValue(filePath);
    }
    if (newNode.hasProperty("originalWidth"))
    {
        newNode.getProperty("originalWidth")->setValue(image.width());
    }
    if (newNode.hasProperty("originalHeight"))
    {
        newNode.getProperty("originalHeight")->setValue(image.height());
    }

    // Add the new node to the list of nodes
    m_nodes.append(newNode);

    update(); // Trigger a repaint
}
void CanvasWidget::createNode(const QString &nodeType, const QString &nodeName)
{
    // Create a placeholder image for the node
    QImage nodeImage(200, 150, QImage::Format_ARGB32_Premultiplied);
    nodeImage.fill(Qt::white);
    QPainter painter(&nodeImage);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 24));
    painter.drawText(nodeImage.rect(), Qt::AlignCenter, nodeType);

    // Generate a unique name if none provided
    QString uniqueName = nodeName;
    if (uniqueName.isEmpty())
    {
        uniqueName = nodeType + "_" + QString::number(++m_nodeCounter);
    }

    // Calculate a position that doesn't overlap with existing nodes
    QPoint position(50 + m_nodeCounter * 30, 50 + m_nodeCounter * 30);

    // Create the node
    Node newNode(nodeImage, position, nodeType, uniqueName);

    // Add the node to the list
    m_nodes.append(newNode);

    update(); // Trigger a repaint
}



Node *CanvasWidget::getSelectedNode()
{
    for (int i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i].isSelected())
        {
            return &m_nodes[i];
        }
    }
    return nullptr;
}
QList<Node *> CanvasWidget::getAllNodes()
{
    QList<Node *> allNodes;
    for (Node &node : m_nodes)
    {
        allNodes.append(&node);
    }
    return allNodes;
}

QImage CanvasWidget::processNodeGraph(Node *outputNode)
{
    if (!outputNode)
        return QImage();

    // Get the child nodes (inputs to the output node)
    QList<Node *> children = outputNode->getChildren();
    if (children.isEmpty())
    {
        qDebug() << "Output node has no connected inputs";
        return QImage();
    }

    // For now, just process the first child - in a complete implementation,
    // you might handle multiple inputs differently
    Node *sourceNode = children.first();

    // Start with the base image if it's an image input node
    if (sourceNode->getType() == "Load Image")
    {
        // Get the file path from the node property
        QString filePath = sourceNode->getProperty("filePath")->getValue().toString();
        if (filePath.isEmpty())
        {
            qDebug() << "File path is empty";
            return QImage();
        }

        // Load the original image
        QImage originalQImage;
        if (!originalQImage.load(filePath))
        {
            qDebug() << "Failed to load image from:" << filePath;
            return QImage();
        }

        // Convert to OpenCV format
        cv::Mat originalCvImage = ImageProcessor::QImageToCvMat(originalQImage);
        cv::Mat processedCvImage = originalCvImage.clone();

        // Process any effects that are connected to this input node
        QList<Node *> effectNodes = sourceNode->getChildren();

        // Apply each effect in sequence
        for (Node *effectNode : effectNodes)
        {
            if (effectNode)
            {
                processedCvImage = ImageProcessor::processNode(effectNode, processedCvImage);
            }
        }

        // Convert back to QImage
        QImage processedQImage = ImageProcessor::CvMatToQImage(processedCvImage);

        // Update the output node preview if it has a preview property
        if (outputNode->hasProperty("preview"))
        {
            double scale = outputNode->getProperty("previewScale")->getValue().toDouble();
            QImage scaledPreview = processedQImage.scaled(
                processedQImage.width() * scale,
                processedQImage.height() * scale,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            outputNode->getProperty("preview")->setValue(QVariant::fromValue(scaledPreview));
        }

        return processedQImage;
    }

    qDebug() << "Source node is not a Load Image node";
    return QImage();
}

void CanvasWidget::saveOutputImage(Node *outputNode, const QString &filePath)
{
    if (!outputNode)
        return;

    QImage processedImage = processNodeGraph(outputNode);
    if (processedImage.isNull())
        return;

    // Get output format from node properties
    QString format = "PNG"; // Default
    int quality = 90;       // Default

    if (outputNode->hasProperty("outputFormat"))
    {
        format = outputNode->getProperty("outputFormat")->getValue().toString();
    }

    if (outputNode->hasProperty("quality"))
    {
        quality = outputNode->getProperty("quality")->getValue().toInt();
    }

    // Save the image
    QString actualFilePath = filePath;
    if (actualFilePath.isEmpty() && outputNode->hasProperty("outputPath"))
    {
        actualFilePath = outputNode->getProperty("outputPath")->getValue().toString();
    }

    if (!actualFilePath.isEmpty())
    {
        // Ensure file has correct extension
        if (!actualFilePath.endsWith("." + format.toLower(), Qt::CaseInsensitive))
        {
            actualFilePath += "." + format.toLower();
        }

        bool saved = processedImage.save(actualFilePath, format.toUtf8().constData(), quality);
        if (saved)
        {
            qDebug() << "Image saved successfully to:" << actualFilePath;
        }
        else
        {
            qDebug() << "Failed to save image to:" << actualFilePath;
        }
    }
}
