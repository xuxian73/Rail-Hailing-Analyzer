#ifndef SEARCHORDER_H
#define SEARCHORDER_H

#include "Mythread.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QElapsedTimer>
#include "math.h"
#include "tool.h"
#include<QDebug>
struct order {
    double departure_lat;
    double departure_lng;
    double end_lat;
    double end_lng;
    int departure_time;
    int travel_time;
    double fee;

    order(double da, double dn, double ea, double en, int dt, int tt, double f){
        departure_lat = da; departure_lng = dn;
        end_lat = ea; end_lng = en;
        departure_time = dt; travel_time = tt;
        fee = f;
    }
};

class SearchOrder : public MyThread
{
    Q_OBJECT
public:
    double departure_lat;
    double departure_lng;
    double end_lat;
    double end_lng;
    qint64 departure_time;
    QVector<order> orderList;
public:
    SearchOrder(QObject* parent);

    void run() override;
};

#endif // SEARCHORDER_H
