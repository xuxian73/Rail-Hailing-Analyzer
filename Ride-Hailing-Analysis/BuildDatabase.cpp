#include "BuildDatabase.h"

void BuildDatabase::run() {
    QElapsedTimer timer;
    timer.start();
    pre_connect_database();
    QSqlDatabase db = get_connection();
    qDebug() << "BuildDatabase triggered";
    if (!db.open()) {
        qDebug() << "Failed to connect db when BuildDatabase: " << db.lastError();
    }
    QSqlQuery q(db);
    initDB(q);
    QString path = Registry::instance()->get("PATH").toString();
    QDir directory(path);
    QStringList fileset = directory.entryList(QStringList() << "order_*", QDir::Files);
    //qDebug() << fileset ;

    QStringList _fileset;
    QString start_date = Registry::instance()->get("start_date").toDate().toString("yyyyMMdd");
    QString end_date = Registry::instance()->get("end_date").toDate().toString("yyyyMMdd");
    for (QString filename: fileset) {
        QString date = filename.mid(6, 8);
        if (date >= start_date and date <= end_date)
            _fileset.append(filename);
    }
    db.transaction();
    for (QString filename : _fileset) {
        //QString filename = _fileset[0];
        QFile file(path + "/" + filename);
        if (!file.open(QIODevice::ReadOnly)){
            qDebug() << "Failed to open file" << filename <<":" << file.error();
            emit description("Failed to open" + filename);
        } else {
            emit description("Loading file " + filename);
            emit progress(double(_fileset.indexOf(filename)) / _fileset.length());
        }
        QTextStream in(&file);
        QString header = in.readLine();
        int col = header.split(",").count();
        QString placeholder = "?";
        for (int i = 1; i < col; ++i, placeholder += ",?");
        placeholder += ",?";
        header += ",travel_time";
        auto statement = QString("insert into origin (%1) values (%2)").arg(header).arg(placeholder);
        q.prepare(statement);
        while (!in.atEnd()) {
            QStringList entry = in.readLine().split(",");
            if (entry[5].toDouble() == 0) continue;

            for (int i = 0; i < col; ++i) {
                q.bindValue(i, entry[i]);
            }


            q.bindValue(col, QString::number(entry[2].toInt() - entry[1].toInt()));
            if(!q.exec()){
                qDebug() << "Failed to insert entry:" << q.lastError();
            }
            if (_cancel) break;
        }
    }
    qDebug() << "BuildDatabase Finished";
    if (_cancel) db.rollback();
    else {
        emit description("Committing changes");
        if (!db.commit()) {
            qDebug() << "Failed to commit: "<< db.lastError();
                    return;
        }
        emit description("ready");
        emit progress(1);
        emit success(true);
    }
    db.close();
    pro_connect_database();
    qDebug() << timer.elapsed() << "ms";
}


BuildDatabase::BuildDatabase(QObject *parent): MyThread(parent){
}

void BuildDatabase::initDB(QSqlQuery &q) {
    emit description("Preparing Database");
    QString statement = "create table origin (order_id text, departure_time integer, end_time integer, orig_lng double,\
                                              orig_lat double, dest_lng double, dest_lat double, fee double, travel_time integer,\
                                              relativity double)";
    if (!q.exec("drop table if exists origin")) {
        qDebug() << "Failed to drop table origin:" << q.lastError();
    }

    if (!q.exec(statement)) {
        qDebug() << "Failed to create table origin when BuildDatabase:" << q.lastError();
    }
}
