#ifndef DRAWHEATMAP_H
#define DRAWHEATMAP_H

#include "Mythread.h"

class DrawHeatmap : public MyThread
{
    Q_OBJECT
public:
    DrawHeatmap(QObject* parent);

    void run() override;
};

#endif // DRAWHEATMAP_H
