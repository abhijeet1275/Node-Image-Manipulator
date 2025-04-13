#pragma once

#include <QString>
#include <QMap>
#include "Node.h"

class NodeManager
{
public:
    static NodeManager &instance()
    {
        static NodeManager instance;
        return instance;
    }

    void addNode(const QString &key, const Node &node);

private:
    QMap<QString, Node> nodes;

    NodeManager() {}
    NodeManager(const NodeManager &) = delete;
    NodeManager &operator=(const NodeManager &) = delete;
};
