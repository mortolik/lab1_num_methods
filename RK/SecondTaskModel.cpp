#include "SecondTaskModel.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <cmath>

using namespace QtCharts;

namespace RungeKutt
{
SecondTask::SecondTask(double A,
                     double B,
                     double C,
                     double Step,
                     int MaxSteps,
                     double Eps,
                     double BoundEps,
                     double StartU,
                     QObject *parent)
    : RungeKutt::TestTaskModel(A, B, Step, MaxSteps,Eps, BoundEps, StartU, parent)
{
    m_parametres.C = C;
}

void SecondTask::runRK4(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi)
{
    series_vi->clear();

    QVector<RungeKutt::DataRow> results;

    for (int i = 1; i <= m_parametres.MAX_STEPS; ++i)
    {
        RungeKutt::DataRow row;
        row.index = i;
        row.X_i = x;
        row.V_i = v;
        row.STEP_i = m_parametres.STEP;

        if (std::isinf(v) || std::isnan(v))
        {
            referenceInfo.IS_INF = true;
            break;
        }

        results.append(row);
        series_vi->append(x, v);

        // Выполняем шаг Рунге-Кутта
        method(x, v, m_parametres.STEP);

        // Проверка выхода за правую границу
        if (x > m_parametres.B)
        {
            double overshoot = x - m_parametres.B;
            x -= overshoot; // Коррекция X
            v = series_vi->pointsVector().last().y(); // Вернуться к последнему V
            break;
        }
    }

    m_results = results;
}

void SecondTask::runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi)
{
    //TODO
}

std::vector<double> SecondTask::f2(double x, std::vector<double> v)
{

}
}
