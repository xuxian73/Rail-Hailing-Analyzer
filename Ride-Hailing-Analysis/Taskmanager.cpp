#include "Taskmanager.h"

TaskManager::TaskManager(QObject* parent):QThread(parent), isRunning(false)
{
}

void TaskManager::run(){
    QEventLoop loop;
    loop.exec();
    if (isRunning) {
        queue.first()->cancel();
        queue.first()->wait();
    }
}

void TaskManager::on_BuildDatabase(BuildDatabase* thread) {
    QMutexLocker l(&queue_mutex);
    if (isRunning and queue.first() == thread) {
        queue.first()->cancel();
        queue.first()->wait();
        queue.clear();
    }
    if (!queue.empty()){
    QList<MyThread*>::iterator it;
        for (it = queue.begin(); it != queue.end(); ++it) {
            if (*it == thread )
                queue.erase(it);
        }
    }
    thread->un_cancel();
    queue.push_back(thread);
    queue.first()->start();
    isRunning = true;
}

void TaskManager::on_PlotChart(PlotChart* thread, qint64 st, qint64 et, unsigned long long ts, int grid){
    QMutexLocker l(&queue_mutex);
    if (queue.empty()) {
        thread->un_cancel();
        thread->start_time = st;
        thread->end_time = et;
        thread->time_step = ts * 60;
        thread->grid = grid;
        queue.push_back(thread);
        queue.first()->start();
        isRunning = true;
        return;
    }
    if (queue.first() == thread) {
        if (isRunning) {
            queue.first()->cancel();
            queue.first()->wait();
            //queue.pop_front();
        }
    } else {
        QList<MyThread*>::iterator it;
        for (it = queue.begin(); it != queue.end(); ++it) {
            if (*it == thread )
                queue.erase(it);
        }
    }
    thread->un_cancel();
    thread->start_time = st;
    thread->end_time = et;
    thread->time_step = ts * 60;
    thread->grid = grid;
    queue.push_back(thread);
}

void TaskManager::on_Distribution(DistributionAnalyze* thread, qint64 st, qint64 et, QString type) {
    QMutexLocker l(&queue_mutex);
    if (queue.empty()) {
        thread->start_time = st;
        thread->end_time = et;
        thread->type = (type == "Travel Time") ? 0 : 1;
        queue.push_back(thread);
        queue.first()->start();
        isRunning = true;
        return;
    }
    if (queue.first() == thread) {
        if (isRunning) {
            queue.first()->cancel();
            queue.first()->wait();
        }
    } else {
        QList<MyThread*>::iterator it;
        for (it = queue.begin(); it != queue.end(); ++it) {
            if (*it == thread )
                queue.erase(it);
        }
    }
    thread->un_cancel();
    thread->start_time = st;
    thread->end_time = et;
    thread->type = (type == "Travel Time") ? 0 : 1;
    queue.push_back(thread);

}

void TaskManager::on_description(QString dcp){
    emit description(dcp);
}

void TaskManager::on_progress(qreal percentage){
    emit progress(percentage);
}

void TaskManager::on_BuildDatabase_success(bool flag){
    QMutexLocker l(&queue_mutex);
    if (flag) {
        solve_finished_task();
        emit BuildDatabase_success();
    } else {
        solve_finished_task();
    }
}

void TaskManager::on_PlotChart_success(bool flag){
    QMutexLocker l(&queue_mutex);
    if (flag){
        solve_finished_task();
        emit PlotChart_success();
    } else {
        solve_finished_task();
    }
}

void TaskManager::on_Ditribution_success(int flag){
    QMutexLocker l(&queue_mutex);
    if (flag) {
        solve_finished_task();
        emit Distribution_success(flag);
    } else {
        solve_finished_task();
    }
}

void TaskManager::solve_finished_task(){
    queue.first()->wait();
    queue.pop_front();
    if(!queue.empty()){
        queue.first()->start();
    } else {
        qDebug() << "Not is Running";
        isRunning = false;
    }
}

void TaskManager::on_SearchOrder(SearchOrder* thread, double departure_lat, double departure_lng,
                                 double end_lat, double end_lng, qint64 departure_time){
    QMutexLocker l(&queue_mutex);
    if (isRunning and queue.first() == thread) {
        queue.first()->cancel();
        queue.first()->wait();
        queue.pop_front();
    }
    if(!queue.empty()){
        QList<MyThread*>::iterator it;
        for (it = queue.begin(); it != queue.end(); ++it) {
            if (*it == thread )
                queue.erase(it);
        }
    }
    thread->un_cancel();
    thread->departure_lat = departure_lat;
    thread->departure_lng = departure_lng;
    thread->end_lat = end_lat;
    thread->end_lng = end_lng;
    thread->departure_time = departure_time;
    queue.push_back(thread);
    if (!isRunning) {
        queue.first()->start();
        isRunning = true;
    }
}

void TaskManager::on_SearchOrder_success(int flag){
    if (flag >= 0) {
        solve_finished_task();
        emit SearchOrder_success(flag);
    } else {
        solve_finished_task();
    }
}
