#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QTranslator>
#include <QDoubleSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSerialData();
    void loadSettings();
    void saveSettings();
    void updateThresholds();
    void changeLanguage(int index);

private:
    void setupUI();
    void setupSerialPort();
    void updateFrequencyDisplay(double freq);

    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTranslator translator;
    double lowerThreshold;
    double upperThreshold;
    QString currentLocale;
};

#endif // MAINWINDOW_H
