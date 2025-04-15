// mainwindow.h
#pragma once
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout> // for addLayout, addStretch etc.
#include <QHBoxLayout>
#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QMap>
#include "node.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleNodeSelected(Node *node);
    void updateNodeProperty(const QString &propertyName, const QVariant &value);
    void handleDeleteChild();

private:
    void setupNodeList();
    void setupAdjustmentPanel(Node *node);
    void clearAdjustmentPanel();
    void updateCombinedGroupProperties(int value);
        QWidget *m_adjustmentsPanel = nullptr;
    Node *m_selectedNode = nullptr;
};
