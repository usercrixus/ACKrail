#include "Topology.hpp"
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <limits>
#include <utility>

Topology::Topology()
    : minimumLatitude(0.0),
      maximumLatitude(0.0),
      minimumLongitude(0.0),
      maximumLongitude(0.0)
{
}

Topology::Topology(QVector<Node> nodes, QVector<Link> links)
    : Topology()
{
    this->nodes = std::move(nodes);
    this->links = std::move(links);
    name = QStringLiteral("Topology");
    if (this->nodes.isEmpty())
        return;

    minimumLatitude = this->nodes.first().getLatitude();
    maximumLatitude = minimumLatitude;
    minimumLongitude = this->nodes.first().getLongitude();
    maximumLongitude = minimumLongitude;
    for (const Node &node : this->nodes)
    {
        minimumLatitude = qMin(minimumLatitude, node.getLatitude());
        maximumLatitude = qMax(maximumLatitude, node.getLatitude());
        minimumLongitude = qMin(minimumLongitude, node.getLongitude());
        maximumLongitude = qMax(maximumLongitude, node.getLongitude());
    }
    resetWeights();
}

QVector<Node> &Topology::getNodes()
{
    return nodes;
}

const QVector<Node> &Topology::getNodes() const
{
    return nodes;
}

QVector<Link> &Topology::getLinks()
{
    return links;
}

const QVector<Link> &Topology::getLinks() const
{
    return links;
}

Node *Topology::findNode(int nodeId)
{
    for (Node &node : nodes)
    {
        if (node.getId() == nodeId)
            return &node;
    }
    return nullptr;
}

const Node *Topology::findNode(int nodeId) const
{
    for (const Node &node : nodes)
    {
        if (node.getId() == nodeId)
            return &node;
    }
    return nullptr;
}

const QString &Topology::getName() const
{
    return name;
}

const QString &Topology::getError() const
{
    return error;
}

double Topology::getMinimumLatitude() const
{
    return minimumLatitude;
}

double Topology::getMaximumLatitude() const
{
    return maximumLatitude;
}

double Topology::getMinimumLongitude() const
{
    return minimumLongitude;
}

double Topology::getMaximumLongitude() const
{
    return maximumLongitude;
}

const std::vector<double> &Topology::getArrivalWeights() const
{
    return arrivalWeights;
}

const std::vector<double> &Topology::getDepartureWeights() const
{
    return departureWeights;
}

struct Bounds
{
    double minimumLatitude = std::numeric_limits<double>::max();
    double maximumLatitude = std::numeric_limits<double>::lowest();
    double minimumLongitude = std::numeric_limits<double>::max();
    double maximumLongitude = std::numeric_limits<double>::lowest();
};

static bool parseNodes(const QJsonArray &values, QVector<Node> &nodes, QHash<int, Node> &nodesById, Bounds &bounds, QString &error)
{
    for (const QJsonValue &value : values)
    {
        const QJsonObject object = value.toObject();
        const int id = object.value(QStringLiteral("id")).toInt(-1);
        const QString name = object.value(QStringLiteral("name")).toString();
        const double latitude = object.value(QStringLiteral("latitude")).toDouble();
        const double longitude = object.value(QStringLiteral("longitude")).toDouble();
        if (id < 0 || name.isEmpty() || nodesById.contains(id))
        {
            error = QStringLiteral("Invalid or duplicate node in topology.");
            return false;
        }
        Node node(id, name, latitude, longitude);
        nodes.append(node);
        nodesById.insert(id, node);
        bounds.minimumLatitude = qMin(bounds.minimumLatitude, latitude);
        bounds.maximumLatitude = qMax(bounds.maximumLatitude, latitude);
        bounds.minimumLongitude = qMin(bounds.minimumLongitude, longitude);
        bounds.maximumLongitude = qMax(bounds.maximumLongitude, longitude);
    }
    if (nodes.isEmpty())
    {
        error = QStringLiteral("The topology contains no nodes.");
        return false;
    }
    return true;
}

static bool parseLinks(const QJsonArray &values, const QHash<int, Node> &nodesById, QVector<Link> &links, QString &error)
{
    QSet<int> linkIds;
    for (const QJsonValue &value : values)
    {
        const QJsonObject object = value.toObject();
        const int id = object.value(QStringLiteral("id")).toInt(-1);
        const int fromNodeId = object.value(QStringLiteral("from")).toInt(-1);
        const int toNodeId = object.value(QStringLiteral("to")).toInt(-1);
        if (id < 0 || linkIds.contains(id) || !nodesById.contains(fromNodeId) || !nodesById.contains(toNodeId))
        {
            error = QStringLiteral("Invalid link or unknown endpoint in topology.");
            return false;
        }
        QString color = object.value(QStringLiteral("color")).toString();
        if (color.isEmpty())
            color = QStringLiteral("#547083");
        const Node &fromNode = nodesById.constFind(fromNodeId).value();
        const Node &toNode = nodesById.constFind(toNodeId).value();
        links.append(Link(id, fromNode, toNode, object.value(QStringLiteral("line")).toString(), std::move(color)));
        linkIds.insert(id);
    }
    return true;
}

static bool readJsonFile(const QString &fileName, QJsonObject &root, QString &error)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        error = QStringLiteral("Cannot open JSON file: %1").arg(fileName);
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        error = QStringLiteral("Invalid JSON: %1").arg(parseError.errorString());
        return false;
    }
    root = document.object();
    return true;
}

bool Topology::load(const QString &fileName)
{
    QJsonObject root;
    QString loadError;
    if (!readJsonFile(fileName, root, loadError))
    {
        clear(loadError);
        return false;
    }
    QVector<Node> loadedNodes;
    QVector<Link> loadedLinks;
    QHash<int, Node> nodesById;
    Bounds bounds;
    if (!parseNodes(root.value(QStringLiteral("nodes")).toArray(), loadedNodes, nodesById, bounds, loadError)
        || !parseLinks(root.value(QStringLiteral("links")).toArray(), nodesById, loadedLinks, loadError))
    {
        clear(loadError);
        return false;
    }
    name = root.value(QStringLiteral("name")).toString(QStringLiteral("Topology"));
    error.clear();
    minimumLatitude = bounds.minimumLatitude;
    maximumLatitude = bounds.maximumLatitude;
    minimumLongitude = bounds.minimumLongitude;
    maximumLongitude = bounds.maximumLongitude;
    nodes = std::move(loadedNodes);
    links = std::move(loadedLinks);
    resetWeights();
    return true;
}

bool Topology::loadWeights(const QString &fileName)
{
    QJsonObject root;
    QString loadError;
    if (!readJsonFile(fileName, root, loadError))
    {
        error = loadError;
        return false;
    }

    QHash<int, Node *> nodesById;
    for (Node &node : nodes)
        nodesById.insert(node.getId(), &node);

    const QJsonArray stations = root.value(QStringLiteral("stations")).toArray();
    if (stations.isEmpty())
    {
        error = QStringLiteral("The weight file contains no stations.");
        return false;
    }

    QHash<int, double> arrivalWeightByNodeId;
    QHash<int, double> departureWeightByNodeId;
    for (const QJsonValue &value : stations)
    {
        const QJsonObject object = value.toObject();
        const int stationId = object.value(QStringLiteral("stationId")).toInt(-1);
        const auto node = nodesById.constFind(stationId);
        if (stationId < 0 || node == nodesById.constEnd())
        {
            error = QStringLiteral("Weight file references an unknown station: %1").arg(stationId);
            resetWeights();
            return false;
        }

        const double arrivalWeight = object.value(QStringLiteral("arrivalWeight")).toDouble(-1.0);
        const double departureWeight = object.value(QStringLiteral("departureWeight")).toDouble(-1.0);
        if (arrivalWeight < 0.0 || departureWeight < 0.0)
        {
            error = QStringLiteral("Invalid station weight for station: %1").arg(stationId);
            resetWeights();
            return false;
        }
        arrivalWeightByNodeId.insert(stationId, arrivalWeight);
        departureWeightByNodeId.insert(stationId, departureWeight);
    }

    arrivalWeights.clear();
    departureWeights.clear();
    arrivalWeights.reserve(static_cast<std::size_t>(nodes.size()));
    departureWeights.reserve(static_cast<std::size_t>(nodes.size()));
    for (const Node &node : nodes)
    {
        arrivalWeights.push_back(arrivalWeightByNodeId.value(node.getId(), 1.0));
        departureWeights.push_back(departureWeightByNodeId.value(node.getId(), 1.0));
    }

    error.clear();
    return true;
}

void Topology::clear(const QString &error)
{
    name.clear();
    this->error = error;
    minimumLatitude = 0.0;
    maximumLatitude = 0.0;
    minimumLongitude = 0.0;
    maximumLongitude = 0.0;
    nodes.clear();
    links.clear();
    arrivalWeights.clear();
    departureWeights.clear();
}

void Topology::resetWeights()
{
    arrivalWeights.assign(static_cast<std::size_t>(nodes.size()), 1.0);
    departureWeights.assign(static_cast<std::size_t>(nodes.size()), 1.0);
}
