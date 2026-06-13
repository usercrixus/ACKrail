#include "TopologyWidget.hpp"
#include "widgets/HeaderWidget.hpp"
#include "widgets/MapWidget.hpp"

#include <QVBoxLayout>

TopologyWidget::TopologyWidget(const Topology &topology, const Garage &garage, QWidget *parent)
    : QWidget(parent),
      headerWidget(new HeaderWidget(topology, garage, this)),
      mapWidget(new MapWidget(topology, garage, this))
{
    setWindowTitle(QStringLiteral("ACKrail"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(headerWidget);
    layout->addWidget(mapWidget, 1);
}

void TopologyWidget::refresh()
{
    headerWidget->update();
    mapWidget->refresh();
}
