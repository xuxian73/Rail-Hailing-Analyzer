#ifndef PLOTCHART_H
#define PLOTCHART_H

#include "Mythread.h"
#include <QSqlError>
#include "tool.h"
#include <QtCharts>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlDriver>
#include <QSqlQueryModel>

extern double coordinate[100][8];
class PlotChart : public MyThread
{

public:
    qint64 start_time;
    qint64 end_time;
    unsigned int time_step;
    int grid;
    QSplineSeries* series;


    PlotChart(QObject* parent);

    void run() override;

};

#endif // PLOTCHART_H
