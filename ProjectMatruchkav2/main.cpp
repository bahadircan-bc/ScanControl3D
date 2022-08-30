#include "mainwindow.h"
#include "visualizer.h"
#include <QApplication>
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QWidget>
//#include <QtWidgets/QHBoxLayout>
//#include <QtWidgets/QVBoxLayout>
//#include <QtWidgets/QPushButton>
//#include <QtWidgets/QRadioButton>
//#include <QtWidgets/QSlider>
//#include <QtWidgets/QGroupBox>
//#include <QtWidgets/QComboBox>
//#include <QtWidgets/QLabel>
//#include <QtWidgets/QMessageBox>
//#include <QtGui/QPainter>
//#include <QtGui/QScreen>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle("Detect Finder");
    w.showMaximized();

    return app.exec();
}
