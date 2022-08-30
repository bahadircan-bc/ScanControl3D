#include "serialcommunicator.h"

SerialCommunicator::SerialCommunicator(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<QList<QSerialPortInfo>>("QList<QSerialPortInfo>");

    serial = new QSerialPort(this);
    configurePort();
}

void SerialCommunicator::stepX(bool right)
{
    if(right)
    {
        writeSerial("x");
        qDebug() << "inside step x (right)";
        readSerial();
        return;
    }

    writeSerial("X");
    qDebug() << "inside step x (left)";
    readSerial();
    return;
}

void SerialCommunicator::stepY(bool right)
{
    if(right)
    {
        writeSerial("y");
        qDebug() << "inside step y (right)";
        readSerial();
        return;
    }

    writeSerial("Y");
    qDebug() << "inside step Y (left)";
    readSerial();
    return;
}

void SerialCommunicator::scanPorts()
{
    auto portList = QSerialPortInfo::availablePorts();
    emit portsFound(portList);
}

void SerialCommunicator::configurePort()
{
    serial->setBaudRate(9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void SerialCommunicator::connectToPort(QString portName)
{
    serial->setPortName(portName);
    if(serial->open(QSerialPort::ReadWrite))
    {
        emit connectionStatus(true);
        return;
    }
    emit connectionStatus(false);
}

void SerialCommunicator::writeSerial(QByteArray data)
{
    if(!serial->isOpen())
        return;
    serial->write(data);
}

bool SerialCommunicator::readSerial()
{
    if(!serial->isOpen())
        return false;

    QByteArray data;
    while(data != "q")
    {
        if(serial->waitForReadyRead(500000))
        {
            data = serial->readAll();
            qDebug() << data;
        }
    }

    emit dataReceived(data);
    return true;
}
