#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <cmath>
#include "TestTaskModel.hpp"

using namespace QtCharts;

namespace RungeKutt
{
RungeKutt::TestTaskModel::TestTaskModel(double A, double B, double Step, int MaxSteps, double Eps, double BoundEps, double StartU, QObject *parent)
    : QObject{parent}
{
    m_parametres.A = A;
    m_parametres.B = B;
    m_parametres.STEP = Step;
    m_parametres.MAX_STEPS = MaxSteps;
    m_parametres.EPS = Eps;
    m_parametres.BOUND_EPS = BoundEps;
    m_parametres.START_U = StartU;
}

double TestTaskModel::f(double x, double v)
{
    return 2 * v;
}

double TestTaskModel::constanta(double x, double v_0)
{
    return v_0 / exp(2 * x);
}

double TestTaskModel::solution(double x, double C)
{
    return exp(2 * x) * C;
}

void RungeKutt::TestTaskModel::method(double X, double V, double STEP, QtCharts::QLineSeries *series_x, QtCharts::QLineSeries *series_dudx)
{
    series_x->clear();
    series_dudx->clear();

    double k1 = f(X, V);
    double k2 = f(X + STEP / 2.0, V + (STEP / 2.0) * k1);
    double k3 = f(X + STEP / 2.0, V + (STEP / 2.0) * k2);
    double k4 = f(X + STEP, V + STEP * k3);

    series_x->append(X, V);
    series_dudx->append(X, f(X, V));

    V += STEP * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
    X += STEP;
}

void RungeKutt::TestTaskModel::runRK4(double x, double v, QtCharts::QLineSeries *series_x, QtCharts::QLineSeries *series_dudx)
{
    series_x->clear();
    series_dudx->clear();

    QVector<DataRow> results;  // Для хранения данных таблицы
    double step = m_parametres.STEP;

    double old_x = x;
    double old_v = v;
    bool exit_flag = false;

    for (int i = 0; i < m_parametres.MAX_STEPS; ++i) {
        // Сохранение текущих значений
        DataRow row;
        row.index = i;
        row.X_i = x;
        row.V_i = v;
        row.V_i_hat = 0;  // Метод без контроля, поэтому V_i^ = 0
        row.V_diff = 0;   // V_i - V_i^ = 0
        row.OLP_S = 0;    // ОЛП(S) = 0
        row.STEP_i = step;
        row.divisions = 0;  // Количество делений = 0
        row.doublings = 0;  // Количество удвоений = 0
        row.U_i = solution(x, constanta(m_parametres.A, m_parametres.START_U));
        row.U_V_diff = std::abs(row.U_i - v);

        results.append(row);  // Добавляем строку в результаты

        // Добавляем точки на графики
        series_x->append(x, v);
        series_dudx->append(x, f(x, v));

        method(x, v, step, series_x, series_dudx);
        if (x > m_parametres.B + m_parametres.BOUND_EPS) {
            x = old_x;
            v = old_v;
            step = m_parametres.B - old_x;
            method(x, v, step, series_x, series_dudx);
            exit_flag = true;
        }

        if (std::abs(x - m_parametres.B) <= m_parametres.BOUND_EPS || exit_flag) {
            break;
        }

        old_x = x;
        old_v = v;
    }

    m_results = results;
}

QVector<DataRow> RungeKutt::TestTaskModel::getResults()
{
    return m_results;
}

}
