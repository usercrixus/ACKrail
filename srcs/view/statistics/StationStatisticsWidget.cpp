#include "StationStatisticsWidget.hpp"

#include <QAbstractItemView>
#include <QFont>
#include <QFormLayout>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

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

StationStatisticsWidget::StationStatisticsWidget(const SimulationStatistics &statistics,
                                                 const Topology &topology,
                                                 QWidget *parent)
    : QWidget(parent),
      statistics(statistics)
{
    stationList = new QListWidget(this);
    stationList->setSelectionMode(QAbstractItemView::SingleSelection);
    stationList->setSortingEnabled(true);
    for (const Node &station : topology.getNodes())
    {
        auto *item = new QListWidgetItem(station.getName(), stationList);
        item->setData(Qt::UserRole, station.getId());
    }
    stationList->sortItems();

    auto *details = new QWidget(this);
    selectedStationName = new QLabel(QStringLiteral("Select a station"), details);
    QFont titleFont = selectedStationName->font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleFont.setBold(true);
    selectedStationName->setFont(titleFont);

    auto *form = new QFormLayout;
    auto addValue = [details, form](const QString &label)
    {
        auto *value = new QLabel(details);
        form->addRow(label, value);
        return value;
    };

    departures = addValue(QStringLiteral("Trip departures"));
    arrivals = addValue(QStringLiteral("Trip arrivals"));
    netFlow = addValue(QStringLiteral("Net engine flow"));
    currentIdle = addValue(QStringLiteral("Current idle engines"));
    averageIdle = addValue(QStringLiteral("Average idle engines"));
    idleRange = addValue(QStringLiteral("Idle engine range"));
    targetIdle = addValue(QStringLiteral("Target idle engines"));
    activityShare = addValue(QStringLiteral("Network activity share"));
    availability = addValue(QStringLiteral("Fleet availability"));
    belowTarget = addValue(QStringLiteral("Time below target"));
    averageTargetGap = addValue(QStringLiteral("Average target gap"));

    auto *detailsLayout = new QVBoxLayout(details);
    detailsLayout->addWidget(selectedStationName);
    detailsLayout->addLayout(form);
    detailsLayout->addStretch();

    auto *splitter = new QSplitter(this);
    splitter->addWidget(stationList);
    splitter->addWidget(details);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("Select a station to display its live statistics."), this));
    layout->addWidget(splitter);

    connect(stationList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *)
            { selectStation(current); });

    auto *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this]()
            {
                if (isVisible())
                    refresh();
            });
    timer->start();
    clearDetails();
}

void StationStatisticsWidget::selectStation(QListWidgetItem *item)
{
    if (item == nullptr)
    {
        selectedStationId = -1;
        selectedStationName->setText(QStringLiteral("Select a station"));
        clearDetails();
        return;
    }

    selectedStationId = item->data(Qt::UserRole).toInt();
    selectedStationName->setText(item->text());
    refresh();
}

void StationStatisticsWidget::refresh()
{
    if (selectedStationId < 0)
        return;

    const StationStatistics &stationStatistics = statistics.getStationStatistics();
    const StationStatistics::StationReport *report =
        stationStatistics.findStationReport(selectedStationId);
    if (report == nullptr)
    {
        clearDetails();
        return;
    }

    departures->setText(formatCount(report->departureCount));
    arrivals->setText(formatCount(report->arrivalCount));
    netFlow->setText(QString::number(static_cast<qlonglong>(report->arrivalCount)
                                    - static_cast<qlonglong>(report->departureCount)));
    currentIdle->setText(formatEngines(report->currentIdleEngines));
    averageIdle->setText(formatEngines(report->averageIdleEngines));
    idleRange->setText(QStringLiteral("%1 – %2")
                           .arg(formatEngines(report->minimumIdleEngines),
                                formatEngines(report->maximumIdleEngines)));
    targetIdle->setText(formatEngines(report->targetIdleEngines));
    const double movements = static_cast<double>(report->departureCount + report->arrivalCount);
    activityShare->setText(formatPercent(
        movements, static_cast<double>(stationStatistics.getTotalMovementCount())));
    availability->setText(formatPercent(
        report->observedTimeSeconds - report->timeWithNoIdleEngineSeconds,
        report->observedTimeSeconds));
    belowTarget->setText(formatPercent(
        report->timeBelowTargetSeconds, report->observedTimeSeconds));
    averageTargetGap->setText(formatEngines(report->averageTargetGap));
}

void StationStatisticsWidget::clearDetails()
{
    const QString placeholder = QStringLiteral("—");
    departures->setText(placeholder);
    arrivals->setText(placeholder);
    netFlow->setText(placeholder);
    currentIdle->setText(placeholder);
    averageIdle->setText(placeholder);
    idleRange->setText(placeholder);
    targetIdle->setText(placeholder);
    activityShare->setText(placeholder);
    availability->setText(placeholder);
    belowTarget->setText(placeholder);
    averageTargetGap->setText(placeholder);
}
