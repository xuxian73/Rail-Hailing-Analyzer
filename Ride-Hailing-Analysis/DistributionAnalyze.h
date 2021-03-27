#ifndef DISTRIBUTIONANALYZE_H
#define DISTRIBUTIONANALYZE_H

#include "Mythread.h"
#include <QSqlError>
#include "tool.h"
#include "registry.h"
#include <QtCharts>

class DistributionAnalyze : public MyThread
{
    Q_OBJECT
signals:

public:
    qint64 start_time;
    qint64 end_time;
    int type;
    QVector<int> v;
    QVector<double> percentage;
public:
    DistributionAnalyze(QObject* parent);

    void run() override;

};

#endif // DISTRIBUTIONANALYZE_H
