#include "HeaderWidget.hpp"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPaintEvent>

HeaderWidget::HeaderWidget(const Topology &topology, const Garage &garage, QWidget *parent)
    : QWidget(parent),
      topology(topology),
      garage(garage)
{
    setFixedHeight(Height);
    refresh();
}

void HeaderWidget::refresh()
{
    activeEngineCount = garage.getActiveEngineCount();
    engineCount = garage.getEngineCount();
    update();
}

void HeaderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(QStringLiteral("#111b25")));
    painter.setPen(QColor(QStringLiteral("#f4f7f9")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 16, QFont::DemiBold));
    painter.drawText(QRectF(24, 0, 100, Height), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("ACKrail"));
    painter.setPen(QColor(QStringLiteral("#91a4b5")));
    painter.setFont(QFont(QStringLiteral("Sans Serif"), 11));
    painter.drawText(QRectF(130, 0, width() - 154, Height), Qt::AlignLeft | Qt::AlignVCenter, topology.getName());
    painter.drawText(QRectF(width() - 260, 0, 236, Height), Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("%1 / %2 engines active").arg(activeEngineCount).arg(engineCount));
}
