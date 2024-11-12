#include "RKWidget.hpp"
#include "RK/TestTaskModel.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDebug>


using namespace QtCharts;

namespace RungeKutt
{


RungeKutt::RKWidget::RKWidget(TestTaskModel *model, QWidget *parent)
    : QWidget{parent}
    , m_rungeKuttModel(model)
    , m_chartX(new QChart())
    , m_chartdudx(new QChart())
    , m_seriesX(new QLineSeries())
    , m_seriesdudx(new QLineSeries())
{
    m_chartX->setTitle("График U(t)");
    m_chartdudx->setTitle("График du/dx");

    m_chartX->addSeries(m_seriesX);
    m_chartdudx->addSeries(m_seriesdudx);

    m_axisX_x = new QValueAxis();
    m_axisX_x->setTitleText("t");
    m_axisX_x->setLabelFormat("%.2f");
    m_chartX->addAxis(m_axisX_x, Qt::AlignBottom);
    m_seriesX->attachAxis(m_axisX_x);

    m_axisY_x = new QValueAxis();
    m_axisY_x->setTitleText("x(t)");
    m_axisY_x->setLabelFormat("%.2f");
    m_chartX->addAxis(m_axisY_x, Qt::AlignLeft);
    m_seriesX->attachAxis(m_axisY_x);

    m_axisX_dudx = new QValueAxis();
    m_axisX_dudx->setTitleText("t");
    m_axisX_dudx->setLabelFormat("%.2f");
    m_chartdudx->addAxis(m_axisX_dudx, Qt::AlignBottom);
    m_seriesdudx->attachAxis(m_axisX_dudx);

    m_axisY_dudx = new QValueAxis();
    m_axisY_dudx->setTitleText("dx/dt");
    m_axisY_dudx->setLabelFormat("%.2f");
    m_chartdudx->addAxis(m_axisY_dudx, Qt::AlignLeft);
    m_seriesdudx->attachAxis(m_axisY_dudx);

    m_chartViewX = new QChartView(m_chartX);
    m_chartViewX->setRenderHint(QPainter::Antialiasing);

    m_chartViewdudx = new QChartView(m_chartdudx);
    m_chartViewdudx->setRenderHint(QPainter::Antialiasing);

    m_aSpinBox = new QDoubleSpinBox();
    m_aSpinBox->setRange(-10.0, 10.0);
    m_aSpinBox->setSingleStep(0.1);
    m_aSpinBox->setPrefix("a = ");

    m_timeSpinBox = new QSpinBox();
    m_timeSpinBox->setRange(1, 1000);
    m_timeSpinBox->setPrefix("Время = ");

    m_updateButton = new QPushButton("Построить");

    connect(m_updateButton, &QPushButton::clicked, this, &RKWidget::updateChart);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_chartViewX);
    mainLayout->addWidget(m_chartViewdudx);
    mainLayout->addWidget(m_aSpinBox);
    mainLayout->addWidget(m_timeSpinBox);
    mainLayout->addWidget(m_updateButton);

    setLayout(mainLayout);
}

void RKWidget::updateChart()
{
    double a = m_aSpinBox->value();
    int maxTime = m_timeSpinBox->value();

    if (maxTime <= 0) {
        maxTime = 1;
        m_timeSpinBox->setValue(maxTime);
    }

    m_rungeKuttModel->setA(a);
    const int maxPoints = 1000;
    double dt = static_cast<double>(maxTime) / maxPoints;
    m_rungeKuttModel->setDt(dt);

    m_seriesX->clear();
    m_seriesdudx->clear();

    //m_rungeKuttModel->method(m_seriesX, m_seriesdudx);

    m_axisX_x->setRange(0, maxTime);
    m_axisX_dudx->setRange(0, maxTime);

    auto pointsX = m_seriesX->pointsVector();
    auto pointsdudx = m_seriesdudx->pointsVector();

    if (!pointsX.empty()) {
        auto [minX, maxX] = std::minmax_element(pointsX.begin(), pointsX.end(),
                                                [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
        m_axisY_x->setRange(minX->y(), maxX->y());
    }

    if (!pointsdudx.empty()) {
        auto [mindudx, maxdudx] = std::minmax_element(pointsdudx.begin(), pointsdudx.end(),
                                                      [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
        m_axisY_dudx->setRange(mindudx->y(), maxdudx->y());
    }
}
}
