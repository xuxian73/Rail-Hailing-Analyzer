#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QString>
#include <QDebug>
#include "registry.h"

class Database
{
public:
    QMutex mutex;

    static void setup(QSqlDatabase& db);

    QSqlDatabase instance();

    Database();

    ~Database();
};

#endif // DATABASE_H
