#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QThread>
#include <QEventLoop>
#include <QList>
#include "Mythread.h"
#include "BuildDatabase.h"
#include "PlotChart.h"
#include "DistributionAnalyze.h"
#include "SearchOrder.h"
#include <QDebug>

class TaskManager : public QThread
{
    Q_OBJECT
signals:
    void description(QString dcp);

    void progress(qreal percentage);

    void BuildDatabase_success();

    void PlotChart_success();

    void Distribution_success(int flag);

    void SearchOrder_success(int flag);

private:
    void solve_finished_task();

public:
    QList<MyThread*> queue;
    QMutex queue_mutex;
    bool isRunning;

    TaskManager(QObject* parent = nullptr);

    void run() override;

    void test_emit(){
        emit description("test");
        emit progress(0.5);
    }

public slots:
    void on_description(QString dcp);

    void on_progress(qreal percentage);

    void on_BuildDatabase(BuildDatabase* thread);

    void on_PlotChart(PlotChart* thread, qint64 st, qint64 et, unsigned long long ts, int grid);

    void on_Distribution(DistributionAnalyze* thread, qint64 st, qint64 et, QString type);

    void on_SearchOrder(SearchOrder* thread, double , double, double, double, qint64);

    void on_BuildDatabase_success(bool flag);

    void on_PlotChart_success(bool flag);

    void on_Ditribution_success(int flag);

    void on_SearchOrder_success(int flag);
};

#endif // TASKMANAGER_H
