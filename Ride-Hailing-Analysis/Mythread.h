#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
class MyThread : public QThread
{
    Q_OBJECT
signals:
    void description(QString dcp);

    void progress(qreal percentage);

    void success(int flag);

    void success_part(qint64);
public:    
    MyThread(QObject *parent = nullptr);

    bool _cancel;

    virtual void run() override;

    void cancel();

    void un_cancel();
};

#endif // MYTHREAD_H
