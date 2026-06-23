#include "DispatchStatisticsWidget.hpp"

#include <QFormLayout>
#include <QTimer>

namespace
{
QString formatCount(std::size_t value)
{
    return QString::number(static_cast<qulonglong>(value));
}
}

DispatchStatisticsWidget::DispatchStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent)
    : QWidget(parent),
      statistics(statistics)
{
    auto *layout = new QFormLayout(this);
    failedPassengerDispatchesLabel = new QLabel(this);
    failedRebalancingDispatchesLabel = new QLabel(this);
    completedPassengerTripsLabel = new QLabel(this);
    completedRebalancingTripsLabel = new QLabel(this);

    layout->addRow(QStringLiteral("Failed passenger dispatches"), failedPassengerDispatchesLabel);
    layout->addRow(QStringLiteral("Failed rebalancing dispatches"), failedRebalancingDispatchesLabel);
    layout->addRow(QStringLiteral("Completed passenger trips"), completedPassengerTripsLabel);
    layout->addRow(QStringLiteral("Completed rebalancing trips"), completedRebalancingTripsLabel);

    auto *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, [this]() { refresh(); });
    timer->start();
    refresh();
}

void DispatchStatisticsWidget::refresh()
{
    completedPassengerTripsLabel->setText(formatCount(statistics.getEngineStatistics().getPassengerTripCount()));
    completedRebalancingTripsLabel->setText(formatCount(statistics.getEngineStatistics().getRebalancingTripCount()));
    failedPassengerDispatchesLabel->setText(formatCount(statistics.getFailedPassengerDispatchCount()));
    failedRebalancingDispatchesLabel->setText(formatCount(statistics.getFailedRebalancingDispatchCount()));
}
