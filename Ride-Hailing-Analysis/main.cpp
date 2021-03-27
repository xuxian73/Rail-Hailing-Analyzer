#include "mainwindow.h"
#include "database.h"
#include <QApplication>

double coordinate[100][8];
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "mydb_cache");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    //db.setDatabaseName(Registry::instance()->get("PATH").toString() + "/mydb.db");
    db.setDatabaseName("file::memory:?cache=shared");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
    if (!db.open()) {
        qDebug() << "Failed to open cache db: " << db.lastError();
    }
    MainWindow w;
    w.show();
    return a.exec();
}
