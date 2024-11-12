#pragma once

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace QtCharts {
class QChartView;
}

class QDoubleSpinBox;
class QSpinBox;
class QPushButton;

namespace RungeKutt
{

class TestTaskModel;
class RKWidget : public QWidget
{
    Q_OBJECT

public:
    RKWidget(TestTaskModel* model, QWidget *parent = nullptr);

private slots:
    void updateChart();

private:
    TestTaskModel *m_rungeKuttModel;
    QtCharts::QChart *m_chartX;
    QtCharts::QChart *m_chartdudx;
    QtCharts::QLineSeries *m_seriesX;
    QtCharts::QLineSeries *m_seriesdudx;
    QtCharts::QChartView *m_chartViewX;
    QtCharts::QChartView *m_chartViewdudx;
    QDoubleSpinBox *m_aSpinBox;
    QSpinBox *m_timeSpinBox;
    QPushButton *m_updateButton;

    QtCharts::QValueAxis *m_axisX_x;
    QtCharts::QValueAxis *m_axisY_x;
    QtCharts::QValueAxis *m_axisX_dudx;
    QtCharts::QValueAxis *m_axisY_dudx;
};
}
