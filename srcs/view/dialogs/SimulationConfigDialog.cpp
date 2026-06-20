#include "SimulationConfigDialog.hpp"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SimulationConfigDialog::SimulationConfigDialog(const QString &topologyFile, const QString &weightFile, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Set simulation"));

    auto *formLayout = new QFormLayout;
    topologyEdit = addPathRow(formLayout, QStringLiteral("Topology"), topologyFile, QStringLiteral("Open topology file"));
    weightEdit = addPathRow(formLayout, QStringLiteral("Weight"), weightFile, QStringLiteral("Open weight file"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addWidget(buttonBox);
    resize(640, sizeHint().height());
}

QString SimulationConfigDialog::getTopologyFile() const
{
    return topologyEdit == nullptr ? QString() : topologyEdit->text().trimmed();
}

QString SimulationConfigDialog::getWeightFile() const
{
    return weightEdit == nullptr ? QString() : weightEdit->text().trimmed();
}

QString SimulationConfigDialog::chooseJsonFile(const QString &title, const QString &currentPath)
{
    const QString startDirectory = currentPath.startsWith(QStringLiteral(":/")) ? QString() : QFileInfo(currentPath).absolutePath();
    return QFileDialog::getOpenFileName(this, title, startDirectory, QStringLiteral("JSON files (*.json);;All files (*)"));
}

QLineEdit *SimulationConfigDialog::addPathRow(QFormLayout *formLayout, const QString &label, const QString &path, const QString &browseTitle)
{
    QLineEdit *pathEdit = new QLineEdit(path, this);
    QPushButton *browseButton = new QPushButton(QStringLiteral("Browse..."), this);

    auto *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(pathEdit, 1);
    pathLayout->addWidget(browseButton);
    formLayout->addRow(label, pathLayout);

    QObject::connect(browseButton, &QPushButton::clicked, this, [this, pathEdit, browseTitle]()
                     {
                         const QString selectedFile = chooseJsonFile(browseTitle, pathEdit->text());
                         if (!selectedFile.isEmpty())
                             pathEdit->setText(selectedFile); });
    return pathEdit;
}
