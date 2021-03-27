#include "ScrollChartView.h"

ScrollChartView::ScrollChartView(QWidget* parent) : QChartView (parent)
{
}

void ScrollChartView::wheelEvent(QWheelEvent *event){
    chart()->scroll(-event->pixelDelta().x(), 0);
    QChartView::wheelEvent(event);
}
