#include "tool.h"

static QMutex database_mutex;
int sqlSize(QSqlQuery query)
{
    int initialPos = query.at();
    int pos = 0;
    if (query.last())
        pos = query.at() + 1;
    else
        pos = 0;
    query.seek(initialPos);
    return pos;
}

void pre_connect_database(){
    database_mutex.lock();
}

void pro_connect_database() {
    database_mutex.unlock();
}

QSqlDatabase get_connection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "mydb");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    //db.setDatabaseName(Registry::instance()->get("PATH").toString() + "/mydb.db");
    db.setDatabaseName("file::memory:?cache=shared");
    return db;
}
