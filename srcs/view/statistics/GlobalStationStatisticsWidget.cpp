#include "GlobalStationStatisticsWidget.hpp"

#include <QFormLayout>
#include <QTimer>

namespace
{
QString formatCount(std::size_t value)
{
    return QString::number(static_cast<qulonglong>(value));
}

QString formatEngines(double value)
{
    return QString::number(value, 'f', 1);
}

QString formatPercent(double part, double total)
{
    const double percent = total <= 0.0 ? 0.0 : part * 100.0 / total;
    return QStringLiteral("%1%").arg(percent, 0, 'f', 1);
}
}

GlobalStationStatisticsWidget::GlobalStationStatisticsWidget(
    const SimulationStatistics &statistics,
    QWidget *parent)
    : QWidget(parent),
      statistics(statistics)
{
    auto *layout = new QFormLayout(this);
    auto addValue = [this, layout](const QString &label)
    {
        auto *value = new QLabel(this);
        layout->addRow(label, value);
        return value;
    };

    stationCount = addValue(QStringLiteral("Stations"));
    currentIdle = addValue(QStringLiteral("Current idle engines"));
    targetIdle = addValue(QStringLiteral("Target idle engines"));
    waitingPassengers = addValue(QStringLiteral("Waiting passengers"));
    stationsWithoutIdle = addValue(QStringLiteral("Stations without idle engines"));
    departures = addValue(QStringLiteral("Trip departures"));
    arrivals = addValue(QStringLiteral("Trip arrivals"));
    averageIdle = addValue(QStringLiteral("Average idle engines per station"));
    averageTargetGap = addValue(QStringLiteral("Average target gap"));
    availability = addValue(QStringLiteral("Network fleet availability"));
    belowTarget = addValue(QStringLiteral("Station time below target"));

    auto *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this]()
            {
                if (isVisible())
                    refresh();
            });
    timer->start();
    refresh();
}

void GlobalStationStatisticsWidget::refresh()
{
    const StationStatistics::GlobalReport report =
        statistics.getStationStatistics().getGlobalReport();

    stationCount->setText(formatCount(report.stationCount));
    currentIdle->setText(formatEngines(report.currentIdleEngines));
    targetIdle->setText(formatEngines(report.targetIdleEngines));
    waitingPassengers->setText(formatCount(report.currentWaitingPassengers));
    stationsWithoutIdle->setText(
        QStringLiteral("%1 / %2")
            .arg(formatCount(report.stationsWithoutIdleEngine),
                 formatCount(report.stationCount)));
    departures->setText(formatCount(report.departureCount));
    arrivals->setText(formatCount(report.arrivalCount));
    averageIdle->setText(formatEngines(report.averageIdleEnginesPerStation));
    averageTargetGap->setText(formatEngines(report.averageTargetGap));
    availability->setText(formatPercent(
        report.observedStationTimeSeconds - report.timeWithNoIdleEngineSeconds,
        report.observedStationTimeSeconds));
    belowTarget->setText(formatPercent(
        report.timeBelowTargetSeconds,
        report.observedStationTimeSeconds));
}
