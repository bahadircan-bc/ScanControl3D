#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
//#include "open3d/Open3D.h"
#include "visualizer.h"
#include "lasercommunicator.h"
#include "serialcommunicator.h"

#include <QScreen>
#include <QString>
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget* container;
    Q3DSurface* surface;
    Visualizer* handler;
    QThread* handlerThread;
    LaserCommunicator* laserX;
    QThread* laserThread;

    void bindProperties();

public slots:
    void updateSensorList(QVector<unsigned int>);
    void laserConnectButtonReleased();
//  disconnect button does not need any parameters
    void printLaserErrCodes(int);
    void debugButtonReleased();
    void portsFound(QList<QSerialPortInfo>);
    void serialConnectButtonReleased();
    void serialLog(bool);

signals:
    void laserConnectButtonReleasedSignal(unsigned int, int);
    void serialConnectButtonReleasedSignal(QString);
    void getProfiles(int);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
