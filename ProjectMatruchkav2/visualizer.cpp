#include "visualizer.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>


Visualizer::Visualizer(Q3DSurface* surface)
    : surface(surface)
{
    dataProxy = new QSurfaceDataProxy();
    dataSeries = new QSurface3DSeries(dataProxy);

    initDataSeries();
}

void Visualizer::initDataSeries()
{
    dataSeries->setDrawMode(QSurface3DSeries::DrawSurface);

    dataSeries->setFlatShadingEnabled(true);
//    surface->axisX()->setLabelFormat("%.2f");
//    surface->axisZ()->setLabelFormat("%.2f");
    surface->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow | QAbstract3DGraph::SelectionSlice);

    fillDataProxy();
    surface->addSeries(dataSeries);
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::black);

    surface->seriesList().at(0)->setBaseGradient(gr);
    surface->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
//    surface->activeTheme()->setType(Q3DTheme::ThemeStoneMoss);
}

void Visualizer::fillDataProxy()
{
    //data -> datarow -> dataarray -> proxy -> QSurface3DSeries
    QString fileadr= "numunetest.csv";
    QString line;
    QStringList proStr;
    int NoP=0;
    int profile_size=1280;
    int rows=0;
    int profiles=0;

    QFile file(fileadr);
    QTextStream in(&file);

    if ( file.open(QFile::ReadOnly | QFile::Text) ) {
    while (!in.atEnd()){
        line = file.readLine();
        proStr = line.split(";");
        NoP++;
    }
    NoP=(NoP-2)/3;
    }

    qDebug()<<profile_size;
    surface->axisX()->setRange(-14.0f,14.0f);
    surface->axisY()->setRange(50.0f, 80.0f);
    surface->axisZ()->setRange(0.0f, (float)NoP/4.5);

    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        /////Open .csv
    while(rows<NoP){

    profiles=0;
    QFile file(fileadr);
    QTextStream in(&file);


    if ( file.open(QFile::ReadOnly | QFile::Text) ) {

             //if .csv open
        QSurfaceDataRow *newRow = new QSurfaceDataRow(profile_size);


        while (!in.atEnd()){
            line = file.readLine();

            if(profiles==rows && profiles<NoP){        //X axis

                proStr = line.split(";");
                proStr.replaceInStrings(",",".");
                int i=0;
                for(; i<proStr.size(); i++){

                    (*newRow)[i].setX(proStr.at(i).toFloat());
                    }
                for(;i>profile_size;i++) (*newRow)[i].setX(0);

                }
            else if(profiles==rows+NoP+1 && profiles<(NoP*2+1)){   //Y axis

                proStr = line.split(";");
                proStr.replaceInStrings(",",".");
                int i=0;
                for(; i<proStr.size(); i++){

                    (*newRow)[i].setZ(proStr.at(i).toFloat());
                    }
                for(;i>profile_size;i++) (*newRow)[i].setZ(0);
            }
            else if(profiles == rows+(NoP*2+2) && profiles < (NoP*3+2)){   //Z axis

                proStr = line.split(';');
                proStr.replaceInStrings(",",".");
                int i=0;
                for(; i<proStr.size(); i++){

                    (*newRow)[i].setY(proStr.at(i).toFloat());
                    }
                for(;i>profile_size;i++) (*newRow)[i].setY(0);
            }

            profiles++;
            }
           *dataArray << newRow;  //profile -> data array
            rows++;
            file.close();
//              dataArray->at(0)->at(0).position().setY(2.0);
            }
}

    dataProxy->resetArray(dataArray);

}





