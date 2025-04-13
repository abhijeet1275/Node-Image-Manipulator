#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QList>
#include <QMouseEvent>
#include "node.h"

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = nullptr);
    void loadImage(const QImage &image);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage m_image;
    QList<Node> m_nodes;
    Node* m_draggedNode = nullptr;  // Currently dragged node
    QPoint m_offset;  // Offset for the mouse inside the node
};

#endif // CANVASWIDGET_H
