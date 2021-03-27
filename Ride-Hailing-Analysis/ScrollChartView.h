#ifndef SCROLLCHARTVIEW_H
#define SCROLLCHARTVIEW_H

#include <QtCharts>
#include <QEvent>

class ScrollChartView : public QChartView
{
    Q_OBJECT
public:
    ScrollChartView(QWidget* parent);

protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // SCROLLCHARTVIEW_H
