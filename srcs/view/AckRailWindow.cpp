#include "AckRailWindow.hpp"

#include "widgets/EmptyPageWidget.hpp"
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

AckRailWindow::AckRailWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("ACKrail"));
    createCentralView();
    createMenus();
}

QString AckRailWindow::chooseJsonFile(const QString &title, const QString &currentPath)
{
    const QString startDirectory = currentPath.startsWith(QStringLiteral(":/"))
                                       ? QString()
                                       : QFileInfo(currentPath).absolutePath();
    return QFileDialog::getOpenFileName(
        this,
        title,
        startDirectory,
        QStringLiteral("JSON files (*.json);;All files (*)"));
}

QLineEdit *AckRailWindow::addPathRow(QFormLayout *formLayout, QDialog *dialog, const QString &label, const QString &path, const QString &browseTitle)
{
    QLineEdit *pathEdit = new QLineEdit(path, dialog);
    QPushButton *browseButton = new QPushButton(QStringLiteral("Browse..."), dialog);

    auto *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(pathEdit, 1);
    pathLayout->addWidget(browseButton);
    formLayout->addRow(label, pathLayout);

    QObject::connect(browseButton, &QPushButton::clicked, dialog, [this, pathEdit, browseTitle]()
                     {
                         const QString selectedFile = chooseJsonFile(browseTitle, pathEdit->text());
                         if (!selectedFile.isEmpty())
                             pathEdit->setText(selectedFile);
                     });

    return pathEdit;
}

void AckRailWindow::showDimOverlay(QWidget &overlay)
{
    overlay.setAttribute(Qt::WA_StyledBackground, true);
    overlay.setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 150);"));
    overlay.setGeometry(rect());
    overlay.show();
    overlay.raise();
}

void AckRailWindow::createCentralView()
{
    centralStack = new QStackedWidget(this);
    emptyPage = new EmptyPageWidget(centralStack);
    simulationPage = nullptr;
    centralStack->addWidget(emptyPage);
    setCentralWidget(centralStack);
}

void AckRailWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    QAction *setSimulationAction = fileMenu->addAction(QStringLiteral("Set &Simulation..."));
    QObject::connect(setSimulationAction, &QAction::triggered, this, [this]()
                     { setSimulation(); });

    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(QStringLiteral("&Quit"));
    QObject::connect(quitAction, &QAction::triggered, this, [this]()
                     { close(); });

    QMenu *simulationMenu = menuBar()->addMenu(QStringLiteral("&Simulation"));
    launchAction = simulationMenu->addAction(QStringLiteral("&Launch Simulation"));
    QObject::connect(launchAction, &QAction::triggered, this, [this]()
                     { launchSimulation(); });

    stopAction = simulationMenu->addAction(QStringLiteral("&Stop Simulation"));
    QObject::connect(stopAction, &QAction::triggered, this, [this]()
                     { stopSimulation(); });

    updateStatus();
}

void AckRailWindow::setSimulation()
{
    const bool simulationWasVisible = simulationPage != nullptr && centralStack->currentWidget() == simulationPage;
    if (simulationWasVisible)
        centralStack->setCurrentWidget(emptyPage);

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Set simulation"));
    QWidget overlay(this);

    const auto restoreSimulationPage = [this, simulationWasVisible]()
    {
        if (simulationWasVisible && simulationPage != nullptr)
            centralStack->setCurrentWidget(simulationPage);
    };

    auto *formLayout = new QFormLayout;
    QLineEdit *topologyEdit = addPathRow(formLayout, &dialog, QStringLiteral("Topology"), topologyFile, QStringLiteral("Open topology file"));
    QLineEdit *weightEdit = addPathRow(formLayout, &dialog, QStringLiteral("Weight"), weightFile, QStringLiteral("Open weight file"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    auto *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addWidget(buttonBox);
    dialog.resize(640, dialog.sizeHint().height());

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    showDimOverlay(overlay);

    if (dialog.exec() != QDialog::Accepted)
    {
        restoreSimulationPage();
        return;
    }

    const QString nextTopologyFile = topologyEdit->text().trimmed();
    if (nextTopologyFile.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("Cannot set simulation"), QStringLiteral("Topology path cannot be empty."));
        restoreSimulationPage();
        return;
    }

    stopSimulation();
    topologyFile = nextTopologyFile;
    weightFile = weightEdit->text().trimmed();
    updateStatus();
}

void AckRailWindow::launchSimulation()
{
    clearSimulation();

    auto nextSession = std::make_unique<SimulationSession>(topologyFile, weightFile, this);
    if (!nextSession->load())
    {
        QMessageBox::warning(this, QStringLiteral("Cannot launch simulation"), nextSession->getError());
        updateStatus();
        return;
    }

    TopologyWidget *widget = nextSession->createWidget(centralStack);
    if (widget == nullptr)
    {
        QMessageBox::warning(this, QStringLiteral("Cannot launch simulation"), QStringLiteral("Simulation view could not be created."));
        updateStatus();
        return;
    }

    simulationPage = widget;
    centralStack->addWidget(simulationPage);
    centralStack->setCurrentWidget(simulationPage);
    simulationSession = std::move(nextSession);
    simulationSession->start();
    updateStatus();
}

void AckRailWindow::stopSimulation()
{
    clearSimulation();
    updateStatus();
}

void AckRailWindow::clearSimulation()
{
    if (centralStack != nullptr && emptyPage != nullptr)
        centralStack->setCurrentWidget(emptyPage);

    if (simulationPage != nullptr)
    {
        centralStack->removeWidget(simulationPage);
        delete simulationPage;
        simulationPage = nullptr;
    }
    simulationSession.reset();
}

void AckRailWindow::updateStatus()
{
    const QString topologyName = topologyFile.startsWith(QStringLiteral(":/")) ? topologyFile : QFileInfo(topologyFile).fileName();
    const QString weightName = weightFile.isEmpty()
                                   ? QStringLiteral("none")
                                   : QFileInfo(weightFile).fileName();
    const QString simulationState = simulationSession == nullptr ? QStringLiteral("stopped") : QStringLiteral("running");
    statusBar()->showMessage(QStringLiteral("Topology: %1 | Weights: %2 | Simulation: %3").arg(topologyName, weightName, simulationState));
    if (launchAction != nullptr)
        launchAction->setEnabled(!topologyFile.isEmpty());
    if (stopAction != nullptr)
        stopAction->setEnabled(simulationSession != nullptr);
}
