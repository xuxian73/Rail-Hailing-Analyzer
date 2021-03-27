#include "database.h"

QMutex db_mutex;
void Database::setup(QSqlDatabase& db) {
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    //db.setDatabaseName(Registry::instance()->get("PATH").toString() + "/mydb.db");
    db.setDatabaseName("file::memory:?cache=shared");
}

QSqlDatabase Database::instance(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "mydb");
    setup(db);
    return db;
}

Database::Database(){
    db_mutex.lock();
}

Database::~Database(){
    QSqlDatabase::removeDatabase("mydb");
    db_mutex.unlock();
}
