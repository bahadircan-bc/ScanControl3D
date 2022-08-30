#ifndef LASERCOMMUNICATOR_H
#define LASERCOMMUNICATOR_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QMetaType>
#include "C_InterfaceLLT_2.h"
#include <fstream>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "serialcommunicator.h"

class LaserCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit LaserCommunicator(QObject *parent = nullptr);

    void dataToCSV(std::vector<double>&, std::vector<double>&, std::vector<unsigned short>, int);

    long pLLT;
    TScannerType scanCONTROLType;
    QVector<unsigned int> Interfaces;
    int iRetValue;
    unsigned int m_uiPacketSize = 1024;
    unsigned int exposureTime = 100;
    unsigned int idleTime = 610;
    unsigned int profileCount = 1;
    std::vector<unsigned char> vucContainerBuffer;
    std::vector<unsigned char> vucVideoBuffer;
    unsigned int fieldCount = 4;

    //initialized during connection
    DWORD resolution;
    DWORD dwRearrangement;

    //serial
    SerialCommunicator* serialCom;

public slots:
    void scanInterfaces();
    void connectToSensor(unsigned int, int);
    void disconnectFromSensor();
    void startProcess();
    void pollData();
    void pollImageData();

    void tempGetData(int x, int y);

signals:
    void devicesFound(QVector<unsigned int>);
    void sendFeedback(int);

    void returnToLimits();
    void stepX(int);
    void stepY(int);
};

#endif // LASERCOMMUNICATOR_H
