#pragma once

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QString>

class SimulationConfigDialog : public QDialog
{
public:
    SimulationConfigDialog(const QString &topologyFile, const QString &weightFile, QWidget *parent = nullptr);

    QString getTopologyFile() const;
    QString getWeightFile() const;

private:
    QString chooseJsonFile(const QString &title, const QString &currentPath);
    QLineEdit *addPathRow(QFormLayout *formLayout, const QString &label, const QString &path, const QString &browseTitle);

    QLineEdit *topologyEdit = nullptr;
    QLineEdit *weightEdit = nullptr;
};
