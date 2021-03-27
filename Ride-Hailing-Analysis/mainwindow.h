#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressBar>
#include <QAction>
#include <QDateTime>
#include <QLayout>
#include <QtWebEngineWidgets>
#include <QtWebChannel>
#include "registry.h"

#include "BuildDatabase.h"
#include "PlotChart.h"
#include "Mythread.h"
#include "Taskmanager.h"
#include "DistributionAnalyze.h"
#include "SearchOrder.h"
#include "jscommunicator.h"
#include "DrawHeatmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void build_database(BuildDatabase* thread);

    void plot_chart(PlotChart* thread, qint64 st, qint64 et, unsigned long long ts, int grid);

    void distribution(DistributionAnalyze* thread, qint64 st, qint64 et, QString type);

    void search_order(SearchOrder* thread, double departure_lat, double departure_lng, double end_lat, double end_lng, qint64 time);

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void BuildDatabase_success();

    void PlotChart_success();

    void Distribution_success(int flag);

    void SearchOrder_success(int flag);

    void on_Apply_clicked();

    void on_open_button_clicked();

    void on_jsChangeMarker(int type, double lat, double lng);

    void setupUI();

    void setupSignal();

    void load_gird();

    void on_success_part(qint64 time_stage);

private:
    Ui::MainWindow *ui;
    QProgressBar *Progress;
    QLabel *Description;
    TaskManager manager;
    BuildDatabase* bdThread;
    PlotChart* pcThread;
    DistributionAnalyze* daThread;
    SearchOrder* soThread;
    DrawHeatmap* dhThread;
    jsCommunicator* js;
    QWebChannel* webchannel;

    QStandardItemModel* model;
    QDateTimeAxis* timeAxis;
    QValueAxis* valueAxis;
    QChart* timechart;

    void draw_fee_distribution();

    void draw_travel_time_distribution();

    void prepare_chart();

private slots:
    void description(QString dcp);

    void progress(qreal percentage);

    void progress_int(int );

    void on_plot_button_clicked();

    void on_query_clicked();

    void on_loadFinished();

    void on_search_button_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

    void on_departure_lat_valueChanged(double arg1);
};
#endif // MAINWINDOW_H
