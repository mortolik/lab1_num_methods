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

void TestTaskModel::updateReferenceInfo(int iterations, double distance, double maxOLP, int doublings, int reductions, double maxStep, double maxStepX, double minStepX, double minStep, double maxError, double maxErrorX)
{
    referenceInfo.iterationsCount = iterations;
    referenceInfo.distanceToBoundary = distance;
    referenceInfo.maxOLP = maxOLP;
    referenceInfo.stepDoublingCount = doublings;
    referenceInfo.stepReductionCount = reductions;
    referenceInfo.maxStep = maxStep;
    referenceInfo.maxStepX = maxStepX;
    referenceInfo.minStepX = minStepX;
    referenceInfo.minStep = minStep;
    referenceInfo.maxError = maxError;
    referenceInfo.maxErrorX = maxErrorX;
}

void RungeKutt::TestTaskModel::method(double &X, double &V, double STEP)
{
    double k1 = f(X, V);
    double k2 = f(X + STEP / 2.0, V + (STEP / 2.0) * k1);
    double k3 = f(X + STEP / 2.0, V + (STEP / 2.0) * k2);
    double k4 = f(X + STEP, V + STEP * k3);

    V += STEP * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
    X += STEP;
}

ReferenceInfo TestTaskModel::getReference() const {
    return referenceInfo;
}

void RungeKutt::TestTaskModel::runRK4(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi) {
    series_ui->clear();
    series_vi->clear();

    QVector<DataRow> results;
    double step = m_parametres.STEP;

    // Рассчитаем константу C для точного решения
    double C = constanta(m_parametres.A, m_parametres.START_U);

    double old_x = x;
    double old_v = v;
    bool exit_flag = false;
    series_ui->append(x, v);  // Точное решение
    series_vi->append(x, v);    // Численное решение

    // Сброс значений структуры ReferenceInfo
    referenceInfo = ReferenceInfo{
        0,                                      // iterationsCount
        0.0,                                   // distanceToBoundary
        0.0,                                   // maxOLP
        0,                                      // stepDoublingCount
        0,                                      // stepReductionCount
        0.0,                                   // maxStep
        0.0,                                   // maxStepX
        0.0,                                   // minStep
        std::numeric_limits<double>::max(),    // minStepX
        0.0,                                   // maxError
        0.0                                    // maxErrorX
    };

    for (int i = 1; i < m_parametres.MAX_STEPS; ++i) {
        // Сохранение данных для таблицы
        DataRow row;
        row.index = i;
        row.X_i = x;
        row.V_i = v;
        row.STEP_i = step;

        // Вычисляем точное решение U_i перед обновлением V_i
        double U_i = solution(x, C);
        row.U_i = U_i;
        row.U_V_diff = std::abs(U_i - v);

        results.append(row);

        // Добавляем данные на график
        series_ui->append(x, U_i);  // Точное решение
        series_vi->append(x, v);    // Численное решение

        // Обновление данных для справки
        referenceInfo.iterationsCount++;
        if (row.U_V_diff > referenceInfo.maxError) {
            referenceInfo.maxError = row.U_V_diff;
            referenceInfo.maxErrorX = x;
        }
        if (step > referenceInfo.maxStep) {
            referenceInfo.maxStep = step;
            referenceInfo.maxStepX = x;
        }
        if (step < referenceInfo.minStep) {
            referenceInfo.minStep = step;
            referenceInfo.minStepX = x;
        }

        // Шаг метода Рунге-Кутта
        method(x, v, step);

        // Проверка выхода за границы
        if (x > m_parametres.B + m_parametres.BOUND_EPS) {
            x = old_x;
            v = old_v;
            step = m_parametres.B - old_x;
            method(x, v, step);
            exit_flag = true;
        }

        if (std::abs(x - m_parametres.B) <= m_parametres.BOUND_EPS || exit_flag) {
            break;
        }

        old_x = x;
        old_v = v;
    }

    // Финальная информация для справки
    referenceInfo.distanceToBoundary = m_parametres.B - old_x;
    m_results = results;
}



void RungeKutt::TestTaskModel::runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi) {
    series_ui->clear();
    series_vi->clear();
    series_ui->append(x, v);  // Точное решение
    series_vi->append(x, v);    // Численное решение

    QVector<DataRow> results;
    double step = m_parametres.STEP;
    double tolerance = m_parametres.EPS;
    double C = constanta(m_parametres.A, m_parametres.START_U);
    double x_half, v_half, x_full, v_full;

    // Переменные для справочной информации
    int doublingCount = 0;
    int reductionCount = 0;
    double maxStep = step;
    double maxStepX = x;
    double minStepX = x;
    double minStep = step;
    double maxOLP = 0.0;
    double maxError = 0.0;
    double maxErrorX = x;
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
        if (row.OLP_S > maxOLP) maxOLP = row.OLP_S;  // Обновляем максимальное значение ОЛП

            // Локальная погрешность
            double S = (v_half - v_full) / (pow(2.0, 4) - 1.0);
            row.OLP_S = std::abs(S)*pow(2.0, 4);

            if (std::abs(S) > tolerance) {
                // Деление шага
                step /= 2.0;
                row.divisions += 1;
            	reductionCount++;
                continue;
            } else if (std::abs(S) < tolerance / pow(2, 5)) {
                // Удвоение шага
                step *= 2.0;
                row.doublings += 1;
            	doublingCount++;
                validStep = true; // Шаг подходит, выходим из цикла
            } else {
                validStep = true; // Шаг подходит, выходим из цикла
            }
        }

        // Обновляем значения после корректного шага
        row.V_i_hat = v_half;
        row.V_diff = std::abs(v_half - v_full);

        x = x_full;
        v = v_full;

        row.X_i = x;
        row.V_i = v;

        double U_i = solution(x, C);
        row.U_i = U_i;
        row.U_V_diff = std::abs(U_i - v);

        // Обновляем максимальную ошибку и X при максимальной разнице между U и V
        if (row.U_V_diff > maxError) {
            maxError = row.U_V_diff;
            maxErrorX = x;
        }

        if (step > maxStep)
        {
            maxStep = step;
            maxStepX = x + step;
        }
        if (step < minStep)
        {
            minStep = step;
            minStepX = x;
        }

        results.append(row);
        series_ui->append(x, U_i);
        series_vi->append(x, v);

        // Проверка выхода за границы
        if (x >= m_parametres.B - m_parametres.BOUND_EPS) {
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
    referenceInfo.maxError = maxError;
    referenceInfo.maxErrorX = maxErrorX;
}

QVector<DataRow> RungeKutt::TestTaskModel::getResults()
{
    return m_results;
}

}
