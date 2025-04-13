#pragma once

#include <QMainWindow>
#include <QString>

struct BlurSettings {
    int radius = 5;
    QString blurType = "Uniform";
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    BlurSettings blurSettings; // <=== Now you can use this in your logic
};
