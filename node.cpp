#include "node.h"

Node::Node(const QImage &image, const QPoint &position, const QString &type, const QString &name)
    : m_image(image), m_position(position), m_dragging(false), m_selected(false),
      m_type(type), m_name(name.isEmpty() ? type : name)
{
    initializeDefaultProperties();
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

QString Node::getType() const
{
    return m_type;
}

void Node::setType(const QString &type)
{
    m_type = type;
}

QString Node::getName() const
{
    return m_name;
}

void Node::setName(const QString &name)
{
    m_name = name;
}

void Node::addProperty(const QString &name, const QVariant &value, NodeProperty::Type type)
{
    if (m_properties.contains(name))
    {
        delete m_properties[name];
    }
    m_properties[name] = new NodeProperty(name, value, type);
}

NodeProperty *Node::getProperty(const QString &name)
{
    return m_properties.value(name, nullptr);
}

QList<NodeProperty *> Node::getAllProperties() const
{
    return m_properties.values();
}

bool Node::hasProperty(const QString &name) const
{
    return m_properties.contains(name);
}

void Node::initializeDefaultProperties()
{
    if (m_type == "Blur")
    {
        addProperty("radius", 5, NodeProperty::Blur_Radius);
        addProperty("blurType", "Uniform", NodeProperty::Enum);
        addProperty("kernel", QImage(), NodeProperty::String); // You may want to make a custom type later

        NodeProperty *blurTypeProp = getProperty("blurType");
        if (blurTypeProp)
        {
            blurTypeProp->setEnumValues({"Uniform", "Directional"});
        }
    }
    else if (m_type == "Sharpen")
    {
        addProperty("amount", 50, NodeProperty::Integer);
        addProperty("Contrast", 2, NodeProperty::Image_contrast);
    }
    else if (m_type == "Grayscale")
    {
        addProperty("method", "Luminosity", NodeProperty::Enum);

        NodeProperty *methodProp = getProperty("method");
        if (methodProp)
        {
            methodProp->setEnumValues({"Average", "Luminosity", "Lightness"});
        }
    }
    else if (m_type == "Brightness")
    {
        addProperty("brightness", 0, NodeProperty::Integer);
        addProperty("contrast", 0, NodeProperty::Integer);
    }
    else if (m_type == "Load Image")
    {
        addProperty("filePath", "", NodeProperty::String);
        addProperty("originalWidth", 0, NodeProperty::Integer);
        addProperty("originalHeight", 0, NodeProperty::Integer);
        addProperty("children", QStringList{}, NodeProperty::CustomList);
    }
    else if (m_type == "Output")
    {
        // Add output node properties
        addProperty("outputFormat", "PNG", NodeProperty::Enum);
        addProperty("quality", 90, NodeProperty::Integer);
        addProperty("outputPath", "", NodeProperty::String);
        addProperty("previewScale", 0.5, NodeProperty::Double);

        // Add a property to store the preview image
        addProperty("preview", QVariant::fromValue(QImage()), NodeProperty::String);

        // Set enum values for output format
        NodeProperty *formatProp = getProperty("outputFormat");
        if (formatProp)
        {
            formatProp->setEnumValues({"PNG", "JPG", "BMP"});
        }
    }
}

// Define the function to accept a Node* and add it to the children list
void Node::addChildNode(Node *child)
{
    m_children.append(child);
}

void Node::removeChildNode(Node *child)
{
    m_children.removeOne(child);
}
QList<Node *> Node::getChildren() const
{
    return m_children;
}
