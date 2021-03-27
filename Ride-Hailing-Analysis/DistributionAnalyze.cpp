#include "DistributionAnalyze.h"

DistributionAnalyze::DistributionAnalyze(QObject* parent) : MyThread(parent)
{

}

void DistributionAnalyze::run(){
    QElapsedTimer timer;
    timer.start();
    pre_connect_database();
    QSqlDatabase db = get_connection();
    if (!db.open()) {
        qDebug() << "Failed to open database" << db.lastError();
    }
    QSqlQuery q(db);
    if (type) {  //plot fee distribution
        if (!Registry::instance()->get("fee").toBool()) {
            emit description("Load data of fee first");
            emit success(false);
            return;
        }
        v.clear();
        percentage.clear();

        QString statement = "select * from origin where :start_time <= departure_time and departure_time < :end_time";
        //statement.append(" order by fee asc");
        q.prepare(statement);
        q.bindValue(":start_time", start_time);
        q.bindValue(":end_time", end_time);
        if (!q.exec()) {
            qDebug() << "Failed to select when analyzing distribution" << q.lastError();
        }
        int index = 1;
        emit description("plotting fee distribution...");
        emit progress(0);
        int total = sqlSize(q);
        /*
        while (q.next() and index <= 20){

            int cnt = 0;
            while(q.value(7) <= index) {
                ++cnt;

                if (!q.next()) break;
                emit progress(double(q.at()) / total);
            }
            q.previous();
            ++index;
            v.append(cnt);
            percentage.append(int(double(cnt) / total * 100));
        }
        */

        statement += " and fee < :index and fee > :index - 1";

        for (int i = 0; i < 20; ++i) {
            QSqlQuery q(db);
            q.prepare(statement);
            q.bindValue(":start_time", start_time);
            q.bindValue(":end_time", end_time);
            q.bindValue(":index", index);
            ++index;
            if (!q.exec()) {
                qDebug() << "Failed to exec:" << q.lastError();
            }
            int cur_cnt = sqlSize(q);
            v.append(cur_cnt);
            percentage.append(int(double(cur_cnt) / total * 100));
            emit progress(double(i) / 20);

            if (_cancel) break;
        }

        qDebug() << v;
        qDebug() << percentage;
        if (_cancel){
            emit description("plot fee distribution cancelled");

        } else {
            emit description("plot fee distribution finished");
            emit progress(1);
            emit success(1);

        }

    } else {  //plot travel_time distribution
        v.clear();
        QString statement = "select * from origin where :start_time <= departure_time and departure_time < :end_time";
        //statement.append(" order by travel_time asc");
        q.prepare(statement);
        q.bindValue(":start_time", start_time);
        q.bindValue(":end_time", end_time);
        if (!q.exec()) {
            qDebug() << "Failed to select when analyzing distribution" << q.lastError();
        }
        int total = sqlSize(q);
        int index = 300;
        v.clear();
        percentage.clear();
        emit description("plotting travel time distribution...");
        emit progress(0);
        /*
        while (q.next()){
            if (total - q.at() < 4) break;
            int cnt = 0;
            while(q.value(8) <= index) {
                ++cnt;
                if (!q.next()) break;
                emit progress(double(q.at()) / total);
            }
            if (_cancel) break;
            q.previous();
            index += 300;
            v.append(cnt);
            percentage.append(int(double(cnt) / total * 100));
        }
        */
        statement += " and travel_time < :index and travel_time > :index - 300";
        for (int i = 0; i < 20; ++i) {
            QSqlQuery q(db);
            q.prepare(statement);
            q.bindValue(":start_time", start_time);
            q.bindValue(":end_time", end_time);
            q.bindValue(":index", index);
            index += 300;
            if (!q.exec()) {
                qDebug() << "Failed to exec:" << q.lastError();
            }
            int cur_cnt = sqlSize(q);
            v.append(cur_cnt);
            percentage.append(int(double(cur_cnt) / total * 100));
            emit progress(double(i) / 20);

            if (_cancel) break;
        }
        qDebug() << v;
        if (_cancel){
            emit description("Plot Travel Time Distribution cancelled");
            emit success(0);
        } else {
            emit description("plot travel time distribution finished");
            emit progress(1);
            emit success(2);
        }

    }
    db.close();
    pro_connect_database();
    qDebug() << timer.elapsed() << "ms";
}

