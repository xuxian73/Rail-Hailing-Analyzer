#ifndef BUILDDATABASE_H
#define BUILDDATABASE_H

#include <QThread>
#include <QDir>
#include <QFile>
#include <QDate>
#include "registry.h"
#include <QDebug>
#include <QSqlError>
#include <QTextStream>
#include <QElapsedTimer>
#include "Mythread.h"
#include "tool.h"

class BuildDatabase : public MyThread{
    Q_OBJECT
public:
    BuildDatabase(QObject *parent = nullptr);

    void run() override;

    void initDB(QSqlQuery& q);
};

#endif //BUILDDATABASE_H
