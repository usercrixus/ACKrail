#include "StationStatisticsWidget.hpp"

#include <QFormLayout>
#include <QTimer>

namespace
{
QString formatEngines(double value)
{
    return QStringLiteral("%1").arg(value, 0, 'f', 1);
}

QString formatSeconds(double seconds)
{
    return QStringLiteral("%1 s").arg(seconds, 0, 'f', 1);
}
}

StationStatisticsWidget::StationStatisticsWidget(const SimulationStatistics &statistics, QWidget *parent)
    : QWidget(parent),
      statistics(statistics)
{
    auto *layout = new QFormLayout(this);
    averageIdleLabel = new QLabel(this);
    minimumAverageIdleLabel = new QLabel(this);
    maximumAverageIdleLabel = new QLabel(this);
    averageTargetGapLabel = new QLabel(this);
    maximumTargetGapLabel = new QLabel(this);
    noEngineTimeLabel = new QLabel(this);
    belowTargetTimeLabel = new QLabel(this);
    aboveTargetTimeLabel = new QLabel(this);

    layout->addRow(QStringLiteral("Average idle engines"), averageIdleLabel);
    layout->addRow(QStringLiteral("Lowest station average"), minimumAverageIdleLabel);
    layout->addRow(QStringLiteral("Highest station average"), maximumAverageIdleLabel);
    layout->addRow(QStringLiteral("Average target gap"), averageTargetGapLabel);
    layout->addRow(QStringLiteral("Maximum target gap"), maximumTargetGapLabel);
    layout->addRow(QStringLiteral("No-engine time"), noEngineTimeLabel);
    layout->addRow(QStringLiteral("Below-target time"), belowTargetTimeLabel);
    layout->addRow(QStringLiteral("Above-target time"), aboveTargetTimeLabel);

    auto *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, [this]() { refresh(); });
    timer->start();
    refresh();
}

void StationStatisticsWidget::refresh()
{
    const StationStatistics &stationStatistics = statistics.getStationStatistics();
    const NumericSummary averageIdleSummary = stationStatistics.getAverageIdleEngineSummary();
    const NumericSummary targetGapSummary = stationStatistics.getTargetGapSummary();

    averageIdleLabel->setText(formatEngines(averageIdleSummary.getAverage()));
    minimumAverageIdleLabel->setText(formatEngines(averageIdleSummary.getMinimum()));
    maximumAverageIdleLabel->setText(formatEngines(averageIdleSummary.getMaximum()));
    averageTargetGapLabel->setText(formatEngines(targetGapSummary.getAverage()));
    maximumTargetGapLabel->setText(formatEngines(targetGapSummary.getMaximum()));
    noEngineTimeLabel->setText(formatSeconds(stationStatistics.getTotalTimeWithNoIdleEngineSeconds()));
    belowTargetTimeLabel->setText(formatSeconds(stationStatistics.getTotalTimeBelowTargetSeconds()));
    aboveTargetTimeLabel->setText(formatSeconds(stationStatistics.getTotalTimeAboveTargetSeconds()));
}
