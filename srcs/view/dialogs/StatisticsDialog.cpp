#include "StatisticsDialog.hpp"

#include "../statistics/DispatchStatisticsWidget.hpp"
#include "../statistics/EngineStatisticsWidget.hpp"
#include "../statistics/StationStatisticsWidget.hpp"

#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>

StatisticsDialog::StatisticsDialog(const SimulationStatistics &statistics, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Simulation statistics"));
    setModal(true);

    auto *tabs = new QTabWidget(this);
    tabs->addTab(new EngineStatisticsWidget(statistics, tabs), QStringLiteral("Engines"));
    tabs->addTab(new StationStatisticsWidget(statistics, tabs), QStringLiteral("Stations"));
    tabs->addTab(new DispatchStatisticsWidget(statistics, tabs), QStringLiteral("Dispatch"));

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
    layout->addWidget(buttonBox);

    resize(560, 440);
}
