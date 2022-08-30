#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    surface = new Q3DSurface();
    QHBoxLayout* layout = new QHBoxLayout();
    container = QWidget::createWindowContainer(surface);

    QSize screenSize = surface->screen()->size();

    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    layout->addWidget(container);
    ui->groupBox->setLayout(layout);

    //handler = new Visualizer(surface);
    //handlerThread = new QThread();
    //handler->moveToThread(handlerThread);


    laserX = new LaserCommunicator(nullptr);
    laserThread = new QThread();
    laserX->moveToThread(laserThread);

    bindProperties();

    laserThread->start();
    //handlerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::bindProperties()
{
    connect(ui->pushButtonScanSensor, &QPushButton::released, laserX, &LaserCommunicator::scanInterfaces);
    connect(laserX, &LaserCommunicator::devicesFound, this, &MainWindow::updateSensorList);
    connect(ui->pushButtonConnectSensor, &QPushButton::released, this, &MainWindow::laserConnectButtonReleased);
    connect(this, &MainWindow::laserConnectButtonReleasedSignal, laserX, &LaserCommunicator::connectToSensor);
    connect(laserX, &LaserCommunicator::sendFeedback, this, &MainWindow::printLaserErrCodes);
    connect(ui->pushButtonDisconnectSensor, &QPushButton::released, laserX, &LaserCommunicator::disconnectFromSensor);
    connect(ui->pushButtonDebug, &QPushButton::released, laserX, &LaserCommunicator::startProcess);
    connect(ui->pushButtonScanPorts, &QPushButton::released, laserX->serialCom, &SerialCommunicator::scanPorts);
    connect(laserX->serialCom, &SerialCommunicator::portsFound, this, &MainWindow::portsFound);
    connect(ui->pushButtonConnectPort, &QPushButton::released, this, &MainWindow::serialConnectButtonReleased);
    connect(this, &MainWindow::serialConnectButtonReleasedSignal, laserX->serialCom, &SerialCommunicator::connectToPort);
    connect(laserX->serialCom, &SerialCommunicator::connectionStatus, this, &MainWindow::serialLog);
}

void MainWindow::updateSensorList(QVector<unsigned int> Interfaces)
{
    ui->listWidgetSensor->clear();
    Q_FOREACH(unsigned int interf, Interfaces)
        ui->listWidgetSensor->addItem(QString::number(interf));
}

void MainWindow::laserConnectButtonReleased()
{
    if(ui->listWidgetSensor->currentItem() == nullptr)
        return;
    unsigned int sensorID = ui->listWidgetSensor->currentItem()->text().toUInt();
    qDebug() << "Signal to try to connect to: " << sensorID;
    emit laserConnectButtonReleasedSignal(sensorID, ui->lineEditNumberOfProfiles->text().toInt());
}

void MainWindow::printLaserErrCodes(int err)
{
    if(err == 1)
        qDebug() << "Function successfully executed";
    else
        qDebug() << "Occured Error Code: " << err;
}

void MainWindow::debugButtonReleased()
{
    emit getProfiles(ui->lineEditNumberOfProfiles->text().toInt());
}

void MainWindow::portsFound(QList<QSerialPortInfo> portList)
{
    ui->listWidgetPorts->clear();
    Q_FOREACH(QSerialPortInfo port, portList)
    {
        ui->listWidgetPorts->addItem(port.portName());
        qDebug() << port.portName();
    }
}

void MainWindow::serialConnectButtonReleased()
{
    if(ui->listWidgetPorts->currentItem() == nullptr)
        return;
    QString portName = ui->listWidgetPorts->currentItem()->text();
    emit serialConnectButtonReleasedSignal(portName);
}

void MainWindow::serialLog(bool status)
{
    qDebug() << status;
}

