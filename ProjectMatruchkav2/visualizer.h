#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QObject>
#include <Q3DSurface>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStringList>
using namespace QtDataVisualization;

class Visualizer : public QObject
{
    Q_OBJECT
public:
    explicit Visualizer(Q3DSurface* surface);

    Q3DSurface* surface;
    QSurfaceDataProxy *dataProxy;
    QSurface3DSeries *dataSeries;

    void initDataSeries();
    void fillDataProxy();


signals:

};

#endif // VISUALIZER_H
