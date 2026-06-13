#include "LinkWidget.hpp"

#include <QColor>
#include <QPen>

LinkWidget::LinkWidget(const Link &link, const MapViewport &viewport)
    : QGraphicsLineItem(QLineF(viewport.mapPosition(link.getFromNode().getLatitude(), link.getFromNode().getLongitude()), viewport.mapPosition(link.getToNode().getLatitude(), link.getToNode().getLongitude())))
{
    QPen linkPen(QColor(link.getColor()), 7, Qt::SolidLine, Qt::RoundCap);
    linkPen.setCosmetic(true);
    setPen(linkPen);
    setZValue(0);
}
