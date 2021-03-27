#include "Mythread.h"

MyThread::MyThread(QObject *parent) : QThread(parent), _cancel(false) {

}

void MyThread::run(){

}

void MyThread::cancel() {
    _cancel = true;
}

void MyThread::un_cancel(){
    _cancel = false;
}
