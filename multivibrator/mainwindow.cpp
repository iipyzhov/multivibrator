#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QDebug>
#include <limits>
#include <QTranslator>
#include <QXmlStreamWriter>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), serial(new QSerialPort(this)) {
    ui->setupUi(this);
    setupUI();
    setupSerialPort();
    loadSettings();

    ui->languageComboBox->addItem("English", "en_US");
    ui->languageComboBox->addItem("Русский", "ru_RU");
    ui->languageComboBox->addItem("Deutsch", "de_DE");

    ui->label->setText(tr("Frequency, (Hz)"));
    ui->label_2->setText(tr("Upper Threshold, (Hz)"));
    ui->label_3->setText(tr("Lower Threshold, (Hz)"));

    // Связываем выбор языка с функцией смены языка
    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::changeLanguage);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::changeLanguage(int index)
{
    currentLocale = ui->languageComboBox->itemData(index).toString();
    if (translator.load("C:\\Qt\\project\\translations\\project_" + currentLocale + ".qm")) {
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }
}


void MainWindow::setupUI() {
    // Устанавливаем минимальные и максимальные значения для doubleSpinBox и doubleSpinBox_2
    ui->doubleSpinBox->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    ui->doubleSpinBox_2->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    // Подключаем сигналы doubleSpinBox и doubleSpinBox_2 к слоту обновления порогов
    connect(ui->doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::updateThresholds);
    connect(ui->doubleSpinBox_2, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::updateThresholds);
}

void MainWindow::setupSerialPort() {
    serial->setPortName("COM4"); // Убедитесь, что выбран правильный COM-порт
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);

    serial->open(QIODevice::ReadOnly); // Открываем порт только для чтения
}

void MainWindow::loadSettings() {
    QSettings settings("C:/Qt/project/build/Desktop_Qt_6_7_1_MinGW_64_bit-Debug/settings.xml", QSettings::NativeFormat);
    lowerThreshold = settings.value("lowerThreshold", 0).toUInt();
    upperThreshold = settings.value("upperThreshold", 1000).toUInt();
    currentLocale = settings.value("language", "en_US").toString(); // Загрузка текущего языка
    ui->doubleSpinBox_2->setValue(lowerThreshold); // Устанавливаем значение нижнего порога в текстовое поле
    ui->doubleSpinBox->setValue(upperThreshold); // Устанавливаем значение верхнего порога в текстовое поле
    // Устанавливаем текущий язык
    int index = ui->languageComboBox->findData(currentLocale);
    if (index != -1) {
        ui->languageComboBox->setCurrentIndex(index);
        changeLanguage(index);
    }
}

void MainWindow::saveSettings() {
    QSettings settings("C:/Qt/project/build/Desktop_Qt_6_7_1_MinGW_64_bit-Debug/settings.xml", QSettings::NativeFormat);
    settings.setValue("lowerThreshold", QString::number(lowerThreshold));
    settings.setValue("upperThreshold", QString::number(upperThreshold));
    settings.setValue("language", currentLocale); // Сохранение текущего языка
    // Записываем пороговые значения в XML файл
    QFile file("C:/Qt/project/build/Desktop_Qt_6_7_1_MinGW_64_bit-Debug/settings.xml");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "<settings>\n";
        out << "    <lowerThreshold>" << lowerThreshold << "</lowerThreshold>\n";
        out << "    <upperThreshold>" << upperThreshold << "</upperThreshold>\n";
        out << "    <windowGeometry>" << QString("%1Х%2").arg(width()).arg(height())<< "</windowGeometry>\n";
        out << "    <windowPosition>" << QString("%1Х%2").arg(geometry().x()).arg(geometry().y())<< "</windowPosition>\n";
        out << "    <locale>" << currentLocale << "</locale>\n";
        out << "</settings>";
        file.close();
    }
}

void MainWindow::updateThresholds() {
    QLocale locale(currentLocale);
    ui->doubleSpinBox_2->setLocale(locale);
    ui->doubleSpinBox->setLocale(locale);
    lowerThreshold = ui->doubleSpinBox_2->value(); // Получаем значение нижнего порога из текстового поля
    upperThreshold = ui->doubleSpinBox->value(); // Получаем значение верхнего порога из текстового поля
}

void MainWindow::readSerialData() {
    QByteArray data = serial->readAll();
    int dataSize = data.size();

    if (dataSize % 2 != 0) {
        return; // Если данные неполные, игнорируем их
    }

    for (int i = 0; i < dataSize; i += 2) {
        char byte1 = data[i+1];
        char byte2 = data[i];

        quint16 value = (static_cast<quint16>(static_cast<uint8_t>(byte1)) << 8) | static_cast<uint8_t>(byte2);

        qDebug() << "Raw data received:" << QByteArray::fromRawData(&byte1, 1).toHex() << QByteArray::fromRawData(&byte2, 1).toHex();
        qDebug() << "Interpreted value:" << value;

        double frequency = 1000000.0*0.92 / (value * 2);
        updateFrequencyDisplay(frequency);
    }
}

void MainWindow::updateFrequencyDisplay(double freq) {
    QString color = "green";
    QLocale locale(currentLocale);
    QString fre = "";
    ui->label_4->setLocale(locale);
    fre = locale.toString(freq);
    ui->label_4->setText(QString("<font color=\"%1\">%2</font>").arg(color).arg(fre));
    ui->doubleSpinBox_2->setLocale(locale);
    ui->doubleSpinBox->setLocale(locale);
    saveSettings();
}
