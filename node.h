#ifndef NODE_H
#define NODE_H

#include <QImage>
#include <QPoint>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include "node_property.h"

class Node
{
public:
    // Constructor
    Node(const QImage &image, const QPoint &position, const QString &type = "default", const QString &name = "");

    // Getter/Setter for node properties
    QImage getImage() const;
    QPoint getPosition() const;
    void setPosition(const QPoint &position);

    bool isDragging() const;
    void setDragging(bool dragging);

    bool isSelected() const;
    void setSelected(bool selected);

    QString getType() const;
    void setType(const QString &type);

    QString getName() const;
    void setName(const QString &name);

    // Node properties management
    void addProperty(const QString &name, const QVariant &value, NodeProperty::Type type);
    NodeProperty *getProperty(const QString &name);
    QList<NodeProperty *> getAllProperties() const;
    bool hasProperty(const QString &name) const;

    // Create default properties based on node type
    void initializeDefaultProperties();

    // Children management
    void addChildNode(Node *child);
    // Add this to the public section of Node class in node.h
    void removeChildNode(Node *child);
    QList<Node *> getChildren() const; // ✅ Added this line

private:
    QImage m_image;
    QPoint m_position;
    bool m_dragging;
    bool m_selected;
    QString m_type;
    QString m_name;
    QList<Node *> m_children;                   // List of child nodes
    QMap<QString, NodeProperty *> m_properties; // Properties for this node
};

#endif // NODE_H
