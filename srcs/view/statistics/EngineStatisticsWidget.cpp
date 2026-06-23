#include "EngineStatisticsWidget.hpp"

#include <QFormLayout>
#include <QTimer>

namespace
{
QString formatCount(std::size_t value)
{
    return QString::number(static_cast<qulonglong>(value));
}

QString formatDistance(double kilometers)
{
    return QStringLiteral("%1 km").arg(kilometers, 0, 'f', 1);
}

QString formatSeconds(double seconds)
{
    return QStringLiteral("%1 s").arg(seconds, 0, 'f', 1);
}
}

EngineStatisticsWidget::EngineStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent)
    : QWidget(parent),
      statistics(statistics)
{
    auto *layout = new QFormLayout(this);
    tripCountLabel = new QLabel(this);
    passengerTripCountLabel = new QLabel(this);
    rebalancingTripCountLabel = new QLabel(this);
    totalDistanceLabel = new QLabel(this);
    passengerDistanceLabel = new QLabel(this);
    rebalancingDistanceLabel = new QLabel(this);
    averageTripDistanceLabel = new QLabel(this);
    maximumTripDistanceLabel = new QLabel(this);
    averageWaitLabel = new QLabel(this);
    maximumWaitLabel = new QLabel(this);

    layout->addRow(QStringLiteral("Trips"), tripCountLabel);
    layout->addRow(QStringLiteral("Passenger trips"), passengerTripCountLabel);
    layout->addRow(QStringLiteral("Rebalancing trips"), rebalancingTripCountLabel);
    layout->addRow(QStringLiteral("Total distance"), totalDistanceLabel);
    layout->addRow(QStringLiteral("Passenger distance"), passengerDistanceLabel);
    layout->addRow(QStringLiteral("Rebalancing distance"), rebalancingDistanceLabel);
    layout->addRow(QStringLiteral("Average trip distance"), averageTripDistanceLabel);
    layout->addRow(QStringLiteral("Maximum trip distance"), maximumTripDistanceLabel);
    layout->addRow(QStringLiteral("Average wait"), averageWaitLabel);
    layout->addRow(QStringLiteral("Maximum wait"), maximumWaitLabel);

    auto *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, [this]() { refresh(); });
    timer->start();
    refresh();
}

void EngineStatisticsWidget::refresh()
{
    const EngineStatistics &engineStatistics = statistics.getEngineStatistics();
    const NumericSummary distanceSummary = engineStatistics.getTotalDistanceSummary();
    const NumericSummary waitSummary = engineStatistics.getWaitTimeSummary();

    tripCountLabel->setText(formatCount(engineStatistics.getTripCount()));
    passengerTripCountLabel->setText(formatCount(engineStatistics.getPassengerTripCount()));
    rebalancingTripCountLabel->setText(formatCount(engineStatistics.getRebalancingTripCount()));
    totalDistanceLabel->setText(formatDistance(engineStatistics.getTotalDistanceKilometers()));
    passengerDistanceLabel->setText(formatDistance(engineStatistics.getPassengerDistanceKilometers()));
    rebalancingDistanceLabel->setText(formatDistance(engineStatistics.getRebalancingDistanceKilometers()));
    averageTripDistanceLabel->setText(formatDistance(distanceSummary.getAverage()));
    maximumTripDistanceLabel->setText(formatDistance(distanceSummary.getMaximum()));
    averageWaitLabel->setText(formatSeconds(waitSummary.getAverage()));
    maximumWaitLabel->setText(formatSeconds(waitSummary.getMaximum()));
}
