#include "SearchOrder.h"

SearchOrder::SearchOrder(QObject* parent): MyThread(parent)
{
}

void SearchOrder::run() {
    QElapsedTimer timer;
    timer.start();
    pre_connect_database();
    QSqlDatabase db = get_connection();
    if (!db.open()){
        qDebug() << "Failed to connect to database when Search Order" << db.lastError();
    }
    emit description("searching order");
    emit progress(0);
    db.transaction();
    QString statement = "select * from origin where \
                        abs(abs(departure_time - :departure_time) % 86400) < 1200";
    QString update_statement = "update origin \
                                set relativity = abs(orig_lat - :departure_lat) + abs(orig_lng - :departure_lng) + abs(dest_lat - :end_lat) + abs(dest_lng - :end_lng) \
                                where abs(abs(departure_time - :departure_time) % 86400) < 1200";
    QSqlQuery q(db);
    q.prepare(update_statement);

    q.bindValue(":departure_lat", departure_lat);
    q.bindValue(":departure_lng", departure_lng);
    q.bindValue(":end_lat", end_lat);
    q.bindValue(":end_lng", end_lng);
    q.bindValue(":departure_time", departure_time);
    qDebug() << "end_lng" << end_lng;
    if (!q.exec()) {
        qDebug() << "Failed to select when Searching Order" << q.lastError();
    }

    /*
    while(q.next()){

        q.value(9) = fabs(departure_lat - q.value(4).toDouble()) +
                     fabs(departure_lng - q.value(3).toDouble()) +
                     fabs(end_lat - q.value(6).toDouble()) +
                     fabs(end_lat - q.value(5).toDouble());
        emit progress(double(q.at()) / total * 0.9);
        if (_cancel) {
            break;
        }
    }
    */
    if (_cancel) {
        db.rollback();
        emit description("Search Order cancelled");
        emit success(-1);

    } else {
        db.commit();
        statement += " order by relativity asc";
        q.prepare(statement);
        q.bindValue(":departure_time", departure_time);
        orderList.clear();
        if  (!q.exec()) {
            qDebug() << "Failed to select when Searching Order" << q.lastError();
        }
        int total = sqlSize(q);
        qDebug() << "total:" << total;
        int tmp = 0;
        while (tmp < 15 and q.next()) {
            orderList.push_back(order(q.value(4).toDouble(),
                                q.value(3).toDouble(),
                                q.value(6).toDouble(),
                                q.value(5).toDouble(),
                                q.value(2).toInt(),
                                q.value(8).toInt(),
                                q.value(7).toDouble()
                                ));
            ++tmp;
            emit progress(0.9 + tmp * 0.1);
        }

        emit progress(1);
        emit description("search order finished");
        emit success(tmp);
    }
    db.close();
    pro_connect_database();
    qDebug() << timer.elapsed() << "ms";
}
