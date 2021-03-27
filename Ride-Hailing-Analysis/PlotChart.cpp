#include "PlotChart.h"

PlotChart::PlotChart(QObject* parent) : MyThread(parent)
{
    series = new QSplineSeries();

}

void PlotChart::run() {
    QElapsedTimer t;
    t.start();

    emit description("connecting database");
    pre_connect_database();
    QSqlDatabase db = get_connection();
    series->clear();
    if (!db.open()) {
        qDebug() << "Failed to connect to database when Plot Chart" << db.lastError();
    }
    emit description("plotting chart");
    emit progress(0);
    QString statement = "select * from origin where :start_time <= departure_time and departure_time < :end_time";
    if (grid >= 0) {
        statement.append(" and orig_lat > :lat_min and orig_lat < :lat_max \
                           and orig_lng > :lng_min and orig_lng < :lng_max");
    }
    statement.append(" order by departure_time asc");
    QSqlQuery q(db);

    q.prepare(statement);
    q.bindValue(":start_time", start_time);
    qDebug() << "start_time" << start_time;
    q.bindValue(":end_time", end_time);
    if (grid >= 0) {
        q.bindValue(":lat_min", coordinate[grid][5]);
        q.bindValue(":lat_max", coordinate[grid][1]);
        q.bindValue(":lng_min", coordinate[grid][0]);
        q.bindValue(":lng_max", coordinate[grid][2]);
    }
    qDebug() << "end_time" << end_time;
    qDebug() << "time_step" << time_step;
    if (!q.exec()) {
        qDebug() << "Failed to select when Plot Chart: " << q.lastError();
    }

    unsigned long long time = start_time + time_step / 2;
    qint64 index = start_time;

    int total_cnt = 0;
    int cnt = 0;
    qint64 tmp = end_time - start_time;
    qint64 time_stage = start_time + 86400;
    while(q.next()){
        ++total_cnt;
        if (q.value(1).toLongLong() < time) {
            ++cnt;
        }
        else {
            series->append(index * 1000, cnt * 3600 / time_step);
            cnt = 1;
            index += time_step;
            time += time_step;
            if (index > time_stage) {
                this->msleep(250);
                emit success_part(time_stage);
                time_stage += 86400;
                this->msleep(250);
            }
            q.previous();
        }
        if (_cancel) break;
        //if ((cnt & 0xf) == 0){
            emit progress(double(index - start_time) / tmp);
        //}
    }
    qDebug() << "total_cnt" << total_cnt;
    if (_cancel) {
        emit description("Plot Chart cancelled");
        qDebug() << "Task plot chart cancelled";
        emit success(false);
    } else{
        emit description("ready");
        emit progress(1);
        emit success(true);
    }
    db.close();
    pro_connect_database();
    qDebug() << t.elapsed() << "ms";
}

