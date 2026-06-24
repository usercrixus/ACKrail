#include "StatisticsDialog.hpp"

#include "../statistics/EngineStatisticsWidget.hpp"
#include "../statistics/GlobalStationStatisticsWidget.hpp"
#include "../statistics/StationStatisticsWidget.hpp"

#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>

StatisticsDialog::StatisticsDialog(const SimulationStatistics &statistics, const Topology &topology, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Simulation statistics"));
    setModal(true);

    auto *tabs = new QTabWidget(this);
    tabs->addTab(new EngineStatisticsWidget(statistics, tabs), QStringLiteral("Engines"));
    tabs->addTab(new GlobalStationStatisticsWidget(statistics, tabs), QStringLiteral("Global Stations"));
    tabs->addTab(new StationStatisticsWidget(statistics, topology, tabs), QStringLiteral("Stations"));

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
    layout->addWidget(buttonBox);

    resize(560, 440);
}
