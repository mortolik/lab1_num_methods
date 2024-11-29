#include "FirstTaskModel.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <cmath>

using namespace QtCharts;

namespace RungeKutt
{
FirstTaskModel::FirstTaskModel(double A,
                     double B,
                     double Step,
                     int MaxSteps,
                     double Eps,
                     double BoundEps,
                     double StartU,
                     QObject *parent)
    : RungeKutt::TestTaskModel(A, B, Step, MaxSteps,Eps, BoundEps, StartU, parent)
{

}

void FirstTaskModel::runRK4(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi)
{
    series_vi->clear();
    series_vi->append(x, v);

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

void FirstTaskModel::runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi)
{
    series_ui->clear();
    series_vi->clear();
    series_vi->append(x, v);

    QVector<DataRow> results;
    double step = m_parametres.STEP;
    double tolerance = m_parametres.EPS;
    double x_half, v_half, x_full, v_full;
    const double MIN_STEP_THRESHOLD = 1e-9; // Минимально допустимый шаг

    // Переменные для справочной информации
    int doublingCount = 0;
    int reductionCount = 0;
    double maxStep = step;
    double maxStepX = x;
    double minStepX = x;
    double minStep = step;
    double maxOLP = 0.0;

    for (int i = 1; i < m_parametres.MAX_STEPS; ++i) {
        DataRow row;
        row.index = i;
        row.STEP_i = step;
        row.divisions = 0;
        row.doublings = 0;

        bool validStep = false; // Флаг для проверки корректности шага
        while (!validStep) {
            x_half = x;
            v_half = v;
            double step_half = step / 2.0;

            // Два шага с половинным шагом
            method(x_half, v_half, step_half);
            method(x_half, v_half, step_half);

            // Один шаг с полным шагом
            x_full = x;
            v_full = v;
            method(x_full, v_full, step);

            // Локальная погрешность
            double S = (v_half - v_full) / (pow(2.0, 4) - 1.0);
            row.OLP_S = std::abs(S) * pow(2.0, 4);

            if (row.OLP_S > maxOLP) {
                maxOLP = row.OLP_S;  // Обновляем максимальное значение ОЛП
            }

            if (std::abs(S) > tolerance) {
                // Деление шага
                step /= 2.0;
                row.divisions += 1;
                reductionCount++;
            } else if (std::abs(S) < tolerance / pow(2, 5)) {
                // Удвоение шага
                step *= 2.0;
                row.doublings += 1;
                doublingCount++;
                validStep = true; // Шаг подходит, выходим из цикла
            } else {
                validStep = true; // Шаг подходит, выходим из цикла
            }
            if (x + step > m_parametres.B) {
                step = m_parametres.B - x; // Корректируем шаг
                method(x, v, step); // Делаем последний шаг до границы
                break;
            }

            // Проверка на минимальный шаг
            if (step < MIN_STEP_THRESHOLD) {
                validStep = true; // Завершаем адаптацию шага
            }
        }

        // Обновляем значения после корректного шага
        row.V_i_hat = v_half;
        row.V_diff = std::abs(v_half - v_full);

        x = x_full;
        v = v_full;

        row.X_i = x;
        row.V_i = v;

        // Обновляем шаги для справочной информации
        if (step > maxStep) {
            maxStep = step;
            maxStepX = x + step;
        }
        if (step < minStep) {
            minStep = step;
            minStepX = x;
        }

        results.append(row);
        series_vi->append(x, v);

        // Проверка выхода за границы
        if (x >= m_parametres.B - m_parametres.BOUND_EPS || step < MIN_STEP_THRESHOLD) {
            break;
        }
    }

    m_results = results;

    // Обновляем справочную информацию
    referenceInfo.iterationsCount = results.size();
    referenceInfo.distanceToBoundary = m_parametres.B - x;
    referenceInfo.maxOLP = maxOLP;
    referenceInfo.stepDoublingCount = doublingCount;
    referenceInfo.stepReductionCount = reductionCount;
    referenceInfo.maxStep = maxStep;
    referenceInfo.maxStepX = maxStepX;
    referenceInfo.minStepX = minStepX;
    referenceInfo.minStep = minStep;
}


double FirstTaskModel::f(double x, double v)
{
    return (pow(x, 3.0) + 1) / (pow(x, 5.0) + 1) * pow(v, 2.0) + v - pow(v, 3.0)*sin(10.0*x);
}
}