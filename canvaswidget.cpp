#include "canvaswidget.h"
#include <QPainter>
#include <QMouseEvent>

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
        painter.drawImage(node.getPosition(), node.getImage());

        if (node.isSelected())
        {
            QRect rect(node.getPosition(), node.getImage().size());
            painter.setPen(QPen(Qt::yellow, 3));
            painter.drawRect(rect);
        }
    }
}
void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    // Check if the mouse click is within any of the nodes
    for (Node &node : m_nodes)
    {
        QRect nodeRect(node.getPosition(), node.getImage().size());
        if (nodeRect.contains(event->pos()))
        {
            node.setDragging(true);
            m_draggedNode = &node; // Store the node being dragged
            break;
        }
    }
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
    if (m_draggedNode)
    {
        m_draggedNode->setDragging(false); // Stop dragging
        m_draggedNode = nullptr;           // Clear the dragged node reference
    }
}

void CanvasWidget::loadImage(const QImage &image)
{
    // Position the node in the center of the canvas
    double scale_factor = 0.2;
    int scaledWidth = image.width() * scale_factor;
    int scaledHeight = image.height() * scale_factor;
    QImage scaledImage = image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // Create a new node with the scaled image

    // Set the position to a fixed point for simplicity
    QPoint position(50, 50); // Initial position
    Node newNode(scaledImage, position);

    // Add the new node to the list of nodes
    m_nodes.append(newNode);

    update(); // Trigger a repaint
}
