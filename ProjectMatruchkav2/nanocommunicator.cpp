#include <QSerialPort>

MySerialPort::MySerialPort()
{
    serial = new QSerialPort(this);
    openSerialPort();
}

void MySerialPort::openSerialPort()
{
    serial->setPortName("COM5");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite))
    {
        //Connected
    }
    else
    {
        //Open error
    }
}

void MySerialPort::writeData(const QByteArray &data)
{
    serial->write(data);
}
