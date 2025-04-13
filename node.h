#ifndef NODE_H
#define NODE_H

#include <QImage>
#include <QPoint>
class Node
{
public:
    Node(const QImage &image, const QPoint &position);

    QImage getImage() const;
    QPoint getPosition() const;
    void setPosition(const QPoint &position);

    bool isDragging() const;
    void setDragging(bool dragging);

    bool isSelected() const;
    void setSelected(bool selected);

private:
    QImage m_image;
    QPoint m_position;
    bool m_dragging;
    bool m_selected = false;
};

#endif // NODE_H
