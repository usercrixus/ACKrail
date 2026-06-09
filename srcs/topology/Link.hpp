#pragma once

#include "Node.hpp"

#include <QString>

class Link
{
public:
    Link(int id, const Node &fromNode, const Node &toNode, QString lineName, QString color);
    int getId() const;
    const Node &getFromNode() const;
    const Node &getToNode() const;
    const QString &getLineName() const;
    const QString &getColor() const;

private:
    int id;
    Node fromNode;
    Node toNode;
    QString lineName;
    QString color;
};
