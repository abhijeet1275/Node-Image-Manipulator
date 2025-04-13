#include "NodeManager.h"

void NodeManager::addNode(const QString &name, const Node &node)
{
    m_nodes.insert(name, node);
}

Node *NodeManager::getNodeByName(const QString &name)
{
    if (m_nodes.contains(name))
    {
        return &m_nodes[name];
    }
    return nullptr; // Return nullptr if the node with the given name does not exist
}

QImage NodeManager::getImageByName(const QString &name)
{
    Node *node = getNodeByName(name);
    if (node)
    {
        return node->getImage();
    }
    return QImage(); // Return an empty image if node doesn't exist
}

void NodeManager::setPositionByName(const QString &name, const QPoint &position)
{
    Node *node = getNodeByName(name);
    if (node)
    {
        node->setPosition(position);
    }
}
