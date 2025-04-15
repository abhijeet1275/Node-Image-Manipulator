#ifndef NODE_PROPERTY_H
#define NODE_PROPERTY_H

#include <QString>
#include <QVariant>

class NodeProperty
{
public:
    enum Type
    {
        Blur_Radius,
        Image_contrast,
        Integer,
        Double,
        String,
        Boolean,
        Enum,
        CustomList,
        ChannelIndex,
    };

    // Constructor
    NodeProperty(const QString &name, const QVariant &value, Type type)
        : m_name(name), m_value(value), m_type(type) {}

    // Getter/Setter for property
    QString getName() const { return m_name; }
    QVariant getValue() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }
    Type getType() const { return m_type; }
    // For enum types
    void setEnumValues(const QStringList &values) { m_enumValues = values; }
    QStringList getEnumValues() const { return m_enumValues; }
private:
    QString m_name;
    QVariant m_value;
    Type m_type;
    QStringList m_enumValues; // For enum types
    // List of child nodes
};

#endif // NODE_PROPERTY_H
