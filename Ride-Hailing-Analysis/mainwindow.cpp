#include "mainwindow.h"
#include "ui_mainwindow.h"

extern double coordinate[100][8];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Rail-Hailing Analyzer");
    setWindowState(Qt::WindowMaximized);
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    model->setColumnCount(8);
    model->setHorizontalHeaderItem(0,new QStandardItem("id"));
    model->setHorizontalHeaderItem(1,new QStandardItem("orig_lat"));
    model->setHorizontalHeaderItem(2,new QStandardItem("orig_lng"));
    model->setHorizontalHeaderItem(3,new QStandardItem("dest_lat"));
    model->setHorizontalHeaderItem(4,new QStandardItem("dest_lng"));
    model->setHorizontalHeaderItem(5,new QStandardItem("departure_time"));
    model->setHorizontalHeaderItem(6,new QStandardItem("travel_time(sec)"));
    model->setHorizontalHeaderItem(7,new QStandardItem("fee"));
    bdThread = new BuildDatabase(this);
    pcThread = new PlotChart(this);
    daThread = new DistributionAnalyze(this);
    soThread = new SearchOrder(this);
    dhThread = new DrawHeatmap(this);
    js = new jsCommunicator();
    webchannel = new QWebChannel();
    webchannel->registerObject("js", js);
    connect(js, &jsCommunicator::jsChangeMarker, this, &MainWindow::on_jsChangeMarker);
    connect(ui->mapView, &QWebEngineView::loadProgress, this, &MainWindow::progress_int);
    connect(ui->mapView, &QWebEngineView::loadFinished, this, &MainWindow::on_loadFinished);
    ui->mapView->load(QUrl("qrc:/web/map.html"));
    ui->mapView->page()->setWebChannel(webchannel);
    setupUI();
    setupSignal();
    prepare_chart();
    ui->item_box->addItem("Travel Time", "TT");
    ui->item_box->addItem("Fee", "F");
    ui->gridBox->addItem("All");
    for (int i = 0; i < 100; ++i) {
        ui->gridBox->addItem(QString::number(i));
    }
    ui->time_step->setValue(5);
    ui->gridBox->setEditable(true);
    ui->gridBox->setMaxVisibleItems(10);
    manager.start();
}

MainWindow::~MainWindow()
{
    manager.quit();
    manager.wait();
    delete ui;
}

void MainWindow::description(QString dcp) {
    Description->setText(dcp);
}

void MainWindow::progress(qreal percentage) {
    Progress->setRange(0,100);
    Progress->setValue(percentage * 100);
}

void MainWindow::progress_int(int i) {
    Progress->setRange(0,100);
    Progress->setValue(i);
}


void MainWindow::on_Apply_clicked()
{
    if (ui->Directory->text() == "") {
        QMessageBox message(QMessageBox::NoIcon, "Warning", "Please set the dataset directory.");
        message.exec();
        return;
    }
    Registry::instance()->set("start_date", ui->start_date->date());
    Registry::instance()->set("end_date", ui->end_date->date());
    Registry::instance()->set("order_id", ui->order_id->isChecked());
    Registry::instance()->set("fee", ui->fee->isChecked());
    ui->Start_time->setMaximumDate(ui->end_date->date());
    ui->End_time->setMaximumDate(ui->end_date->date());
    ui->End_time->setDateTime(ui->End_time->maximumDateTime());
    ui->Start_time->setMinimumDate(ui->start_date->date());
    ui->End_time->setMinimumDate(ui->start_date->date());

    ui->from_time->setMaximumDate(ui->end_date->date());
    ui->from_time->setMinimumDate(ui->start_date->date());
    ui->to_time->setMaximumDate(ui->end_date->date());
    ui->to_time->setMinimumDate(ui->start_date->date());
    ui->to_time->setDateTime(ui->to_time->maximumDateTime());
    load_gird();
    emit build_database(bdThread);

}

void MainWindow::on_open_button_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Dataset Directory");
    ui->Directory->setText(path);
    Registry::instance()->set("PATH", path);
}


void MainWindow::load_gird(){

    QFile file(Registry::instance()->get("PATH").toString() + "/rectangle_grid_table.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open grid_file: " << file.fileName();
    }

    QTextStream in(&file);
    QString header = in.readLine();
    int row = 0;
    while (!in.atEnd()) {
        auto ctx = in.readLine().split(",");
        //qDebug() << ctx;
        for (int i = 0; i < 8; ++i) {
            coordinate[row][i] = ctx[i+1].toDouble();
        }
        ++row;
    }
    //qDebug() << coordinate[99][7];
}

void MainWindow::setupUI(){
    ui->Apply->lower();
    ui->Date->raise();
    ui->Field->raise();
    Description = new QLabel(this);
    Description->setText("status");
    Progress = new QProgressBar(this);
    Progress->setMaximum(100);
    Progress->setMinimum(0);
    ui->statusbar->addPermanentWidget(Progress,1);
    ui->statusbar->addPermanentWidget(Description,0);
    ui->time_step->setMinimum(1);
}

void MainWindow::on_plot_button_clicked()
{
    QDateTime timestamp = ui->Start_time->dateTime();
    QDateTime etimestamp = timestamp.addDays(1);
    if (etimestamp > ui->End_time->dateTime())
        etimestamp = ui->End_time->dateTime();
    timeAxis->setMin(timestamp);
    timeAxis->setMax(etimestamp);
    timeAxis->setTickCount(10);
    if (ui->gridBox->currentText() != "All")
        valueAxis->setMax(1500);
    else valueAxis->setMax(15000);
    valueAxis->setMin(0);

    emit plot_chart(pcThread,
                    ui->Start_time->dateTime().toSecsSinceEpoch(),
                    ui->End_time->dateTime().toSecsSinceEpoch(),
                    ui->time_step->value(),
                    ui->gridBox->currentText() == "All" ? -1 :ui->gridBox->currentText().toInt());
}

void MainWindow::BuildDatabase_success(){

}

void MainWindow::PlotChart_success(){

}

void MainWindow::Distribution_success(int flag){

    if (flag == 1) {
        draw_fee_distribution();
    } else if (flag == 2){
        draw_travel_time_distribution();
    }

}

void MainWindow::setupSignal(){
    connect(bdThread, &MyThread::success, &manager, &TaskManager::on_BuildDatabase_success);
    connect(bdThread, &MyThread::progress, &manager, &TaskManager::on_progress);
    connect(bdThread, &MyThread::description, &manager, &TaskManager::on_description);
    connect(pcThread, &MyThread::success, &manager, &TaskManager::on_PlotChart_success);
    connect(pcThread, &MyThread::progress, &manager, &TaskManager::on_progress);
    connect(pcThread, &MyThread::description, &manager, &TaskManager::on_description);
    connect(daThread, &MyThread::success, &manager, &TaskManager::on_Ditribution_success);
    connect(daThread, &MyThread::description, &manager, &TaskManager::on_description);
    connect(daThread, &MyThread::progress, &manager, &TaskManager::on_progress);
    connect(soThread, &MyThread::success, &manager, &TaskManager::on_SearchOrder_success);
    connect(soThread, &MyThread::description, &manager, &TaskManager::on_description);
    connect(soThread, &MyThread::progress, &manager, &TaskManager::on_progress);
    connect(this, &MainWindow::build_database, &manager, &TaskManager::on_BuildDatabase);
    connect(this, &MainWindow::plot_chart, &manager, &TaskManager::on_PlotChart);
    connect(this, &MainWindow::distribution, &manager, &TaskManager::on_Distribution);
    connect(this, &MainWindow::search_order, &manager, &TaskManager::on_SearchOrder);
    connect(&manager, &TaskManager::BuildDatabase_success, this, &MainWindow::BuildDatabase_success);
    connect(&manager, &TaskManager::PlotChart_success, this, &MainWindow::PlotChart_success);
    connect(&manager, &TaskManager::Distribution_success, this, &MainWindow::Distribution_success);
    connect(&manager, &TaskManager::SearchOrder_success, this, &MainWindow::SearchOrder_success);
    connect(&manager, &TaskManager::description, this, &MainWindow::description);
    connect(&manager, &TaskManager::progress, this, &MainWindow::progress);
    connect(pcThread, &MyThread::success_part, this, &MainWindow::on_success_part);
}

void MainWindow::on_query_clicked()
{
    if (ui->item_box->currentText() == "Fee" and !Registry::instance()->get("fee").toBool()) {
        QMessageBox message(QMessageBox::NoIcon, "confirm", "Please load data about fee at first.");
        message.exec();
        return;
    }
    emit distribution(daThread,
                      ui->from_time->dateTime().toSecsSinceEpoch(),
                      ui->to_time->dateTime().toSecsSinceEpoch(),
                      ui->item_box->currentText());
}

void MainWindow::draw_fee_distribution(){
    QBarSeries *series = new QBarSeries();
    QBarSeries* pseries = new QBarSeries();

    QStringList categories;
    QBarSet* set = new QBarSet("number of people");
    QBarSet* pset = new QBarSet("percentage");
    pset->setLabelColor(Qt::black);
    set->setLabelColor(Qt::black);
    int max = 0;
    for (int i = 0; i < daThread->v.size(); ++i) {
         categories << ("[" + QString::number(i) + "," + QString::number(i+1) + "]");
         max = max  < daThread->v[i] ? daThread->v[i] : max;
    }
    series->append(set);
    pseries->append(pset);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(pseries);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setTitleText("Fee");
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    pseries->attachAxis(axisX);

    series->setLabelsVisible(true);
    pseries->setLabelsVisible(true);

    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    pseries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QValueAxis *axisP = new QValueAxis();
    axisP->setMax(100);
    chart->addAxis(axisP, Qt::AlignRight);
    pseries->attachAxis(axisP);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setTitle("Fee Distribution");
    axisX->setMin("[1,2]");
    axisX->setMax("[11,12]");
    axisY->setMax(max * 1.1);
    ui->distributionChart->setChart(chart);
    for (int i = 0; i < daThread->v.size(); ++i) {
        *set << daThread->v[i];
        *pset << daThread->percentage[i];

    }
}

void MainWindow::draw_travel_time_distribution(){
    QBarSeries *series = new QBarSeries();
    QBarSeries* pseries = new QBarSeries();

    QStringList categories;
    QBarSet* set = new QBarSet("number of people");
    QBarSet* pset = new QBarSet("percentage");
    pset->setLabelColor(Qt::black);
    set->setLabelColor(Qt::black);
    for (int i = 0; i < daThread->v.size(); ++i) {
         categories << ("[" + QString::number(i*5) + "," + QString::number(i*5+5) + "]");
         *set << daThread->v[i];
         *pset << daThread->percentage[i];
    }
    series->append(set);
    pseries->append(pset);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    pseries->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    series->setLabelsVisible(true);
    pseries->setLabelsVisible(true);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(pseries);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    axisX->setTitleText("Travel Time");
    axisX->append(categories);
    axisX->setMax(categories.last());

    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    pseries->attachAxis(axisX);


    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    QValueAxis *axisP = new QValueAxis();
    axisP->setMax(100);
    chart->addAxis(axisP, Qt::AlignRight);
    pseries->attachAxis(axisP);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setTitle("Travel Time Distribution");
    ui->distributionChart->setChart(chart);
    axisX->setMax("[45,50]");
    axisY->setMax(axisY->max() * 1.1);
}

void MainWindow::on_loadFinished() {
    description("load map finished");
}

void MainWindow::on_jsChangeMarker(int type, double lat, double lng){
    if (type == 0) {
        ui->departure_lat->setValue(lat);
        ui->departure_lng->setValue(lng);
    } else if (type == 1) {
        ui->end_lat->setValue(lat);
        ui->end_lng->setValue(lng);
    }
}

void MainWindow::on_search_button_clicked()
{

    double max_lat = 30.793878050323900, min_lat = 30.524081949676100;
    double max_lng = 104.22204452546900, min_lng = 103.90840747453100;

    if (ui->departure_lat->value() >= max_lat || ui->departure_lat->value() <= min_lat
            || ui->departure_lng->value() >= max_lng || ui->departure_lng->value() <= min_lng) {
        QMessageBox message(QMessageBox::NoIcon, "Warning", "The departure position is out of range");
        message.exec();
        return;
    }
    if (ui->end_lat->value() >= max_lat || ui->end_lat->value() <= min_lat
            || ui->end_lng->value() >= max_lng || ui->end_lng->value() <= min_lng) {
        QMessageBox message(QMessageBox::NoIcon, "Warning", "The end position is out of range");
        message.exec();
        return;
    }
    qDebug() <<"search_order triggerred";
    qint64 tmp = ui->departure_time->dateTime().toSecsSinceEpoch();
    qDebug() << tmp;
    emit search_order(soThread, ui->departure_lat->value(), ui->departure_lng->value(),
                      ui->end_lat->value(), ui->end_lng->value(), tmp);

}

void MainWindow::SearchOrder_success(int flag){
    model->setRowCount(flag);
    qDebug() << "flag:" << flag;
    double avg_time = 0, avg_fee = 0;
    for (int i = 0; i < flag; ++i) {
        model->setItem(i, 0, new QStandardItem(QString::number(i+1)));
        model->setItem(i, 1, new QStandardItem(QString::number(soThread->orderList[i].departure_lat)));
        model->setItem(i, 2, new QStandardItem(QString::number(soThread->orderList[i].departure_lng)));
        model->setItem(i, 3, new QStandardItem(QString::number(soThread->orderList[i].end_lat)));
        model->setItem(i, 4, new QStandardItem(QString::number(soThread->orderList[i].end_lng)));
        QDateTime tmp;
        tmp.setSecsSinceEpoch(qint64(soThread->orderList[i].departure_time));
        model->setItem(i, 5, new QStandardItem(tmp.toString("yyyy-MM-dd-hh:mm")));
        model->setItem(i, 6, new QStandardItem(QString::number(soThread->orderList[i].travel_time)));
        model->setItem(i, 7, new QStandardItem(QString::number(soThread->orderList[i].fee)));
        avg_time += model->item(i, 6)->text().toDouble();
        avg_fee += model->item(i, 7)->text().toDouble();
    }
    ui->avg_time->setText(QString::number(avg_time / flag,'g',6));
    ui->avg_fee->setText(QString::number(avg_fee / flag,'g',4));

}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{

}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int row = index.row();
    ui->tableView->selectRow(row);
    qDebug() << "click event triggered";
    ui->mapView->page()->runJavaScript("delMarker('departure_pos(order)')");
    ui->mapView->page()->runJavaScript("delMarker('end_pos(order)')");
    QJsonObject json;
    json["departure_lat"] = model->item(row, 1)->text();
    json["departure_lng"] = model->item(row, 2)->text();
    json["end_lat"] = model->item(row, 3)->text();
    json["end_lng"] = model->item(row, 4)->text();
    json["departure_time"] = model->item(row, 5)->text();
    json["travel_time"] = model->item(row,6)->text();
    json["fee"] = model->item(row, 7)->text();
    qDebug() << json;
    ui->mapView->page()->runJavaScript(QString("addOrderMarker( 'departure_pos(order)',%1, %2, '%3', %4)").arg(json["departure_lat"].toString(),
                                       json["departure_lng"].toString(), json["departure_time"].toString(), json["fee"].toString()));
    ui->mapView->page()->runJavaScript(QString("addOrderMarker( 'end_pos(order)',%1, %2, '%3', %4)").arg(json["end_lat"].toString(),
                                       json["end_lng"].toString(), json["departure_time"].toString(), json["fee"].toString()));

}

void MainWindow::prepare_chart(){
    timechart = new QChart();
    timechart->addSeries(pcThread->series);
    timeAxis = new QDateTimeAxis;
    timeAxis->setLabelsAngle(-45);
    timeAxis->setFormat("yyyy-MM-dd h:mm");
    valueAxis = new QValueAxis;
    valueAxis->setMax(15000);
    timechart->addAxis(timeAxis, Qt::AlignBottom);
    timechart->addAxis(valueAxis, Qt::AlignLeft);
    pcThread->series->attachAxis(timeAxis);
    pcThread->series->attachAxis(valueAxis);
    ui->timeChart->setChart(timechart);
    pcThread->series->setName("order per hour");
    timechart->legend()->setVisible(true);
    ui->timeChart->setRubberBand(QChartView::RectangleRubberBand);
    ui->timeChart->show();
}


void MainWindow::on_departure_lat_valueChanged(double arg1)
{
}

void MainWindow::on_success_part(qint64 time_stage){
    QDateTime etimestamp;
    etimestamp.setSecsSinceEpoch(time_stage + 86400 > pcThread->end_time ? pcThread->end_time : time_stage + 86400);
    QDateTime timestamp;
    timestamp = etimestamp.addDays(-1);
    timeAxis->setMin(timestamp);
    timeAxis->setMax(etimestamp);
}

