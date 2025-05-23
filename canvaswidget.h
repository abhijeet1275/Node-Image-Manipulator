// canvaswidget.h
#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QList>
#include <QMouseEvent>
#include "node.h"
#include "image_processor.h"
#include <QStack>
#include <QDebug>
#include <QPoint>

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    void loadImage(const QImage &image, const QString &filePath = "");
    void createNode(const QString &nodeType, const QString &nodeName = "");
    const QList<Node> &getNodes() const { return m_nodes; }
    Node *getSelectedNode();
    QList<Node *> getAllNodes();
    QImage processNodeGraph(Node *outputNode);
    void saveOutputImage(Node *outputNode, const QString &filePath);
    void clear();
    void removeNode(Node *childNode);
    void undo();
    void redo();
    void zoomIn();
    void zoomOut();
    void resetZoom();
signals:
    void nodeSelected(Node *node);
    void nodePropertyChanged(Node *node, const QString &propertyName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    

private:
    QList<Node> m_nodes;
    Node *m_draggedNode = nullptr; // Currently dragged node
    QPoint m_offset;               // Offset for the mouse inside the node
    int m_nodeCounter = 0;         // For generating unique node names
    QStack<QList<Node>> m_undoStack; // Stack for undo functionality
    QStack<QList<Node>> m_redoStack; // Stack for redo functionality

};

#endif // CANVASWIDGET_H
