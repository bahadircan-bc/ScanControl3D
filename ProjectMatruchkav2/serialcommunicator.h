#ifndef SERIALCOMMUNICATOR_H
#define SERIALCOMMUNICATOR_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class SerialCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunicator(QObject *parent = nullptr);

    QSerialPort* serial;

    void stepX(bool);
    void stepY(bool);

    public slots:
    void scanPorts();
    void configurePort();
    void connectToPort(QString);
    //void disconnectFromPort();
    void writeSerial(QByteArray);
    bool readSerial();


signals:
    void dataReceived(QByteArray);
    void connectionStatus(bool);
    void portsFound(QList<QSerialPortInfo>);

};

#endif // SERIALCOMMUNICATOR_H
