#include "node.h"

Node::Node(const QImage &image, const QPoint &position)
    : m_image(image), m_position(position), m_dragging(false), m_selected(false)
{
}

QImage Node::getImage() const
{
    return m_image;
}

QPoint Node::getPosition() const
{
    return m_position;
}

void Node::setPosition(const QPoint &position)
{
    m_position = position;
}

bool Node::isDragging() const
{
    return m_dragging;
}

void Node::setDragging(bool dragging)
{
    m_dragging = dragging;
}

bool Node::isSelected() const
{
    return m_selected;
}

void Node::setSelected(bool selected)
{
    m_selected = selected;
}
