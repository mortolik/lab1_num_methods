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

void RungeKutt::TestTaskModel::method(double &X, double &V, double STEP)
{
    double k1 = f(X, V);
    double k2 = f(X + STEP / 2.0, V + (STEP / 2.0) * k1);
    double k3 = f(X + STEP / 2.0, V + (STEP / 2.0) * k2);
    double k4 = f(X + STEP, V + STEP * k3);

    V += STEP * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
    X += STEP;
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

    for (int i = 0; i < m_parametres.MAX_STEPS; ++i) {
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

    m_results = results;
}

void RungeKutt::TestTaskModel::runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi) {
    series_ui->clear();
    series_vi->clear();

    QVector<DataRow> results;
    double step = m_parametres.STEP;
    double tolerance = m_parametres.EPS;
    double C = constanta(m_parametres.A, m_parametres.START_U);


    for (int i = 1; i < m_parametres.MAX_STEPS && x <= m_parametres.B; ++i) {
        DataRow row;
        row.index = i;
        row.STEP_i = step;
        row.divisions = 0;
        row.doublings = 0;

        double x_half = x, v_half = v;
        double step_half = step / 2.0;

        // Два шага с половинным шагом
        method(x_half, v_half, step_half);
        method(x_half, v_half, step_half);

        // Один шаг с полным шагом
        double x_full = x, v_full = v;
        method(x_full, v_full, step);

        // Локальная погрешность
        double S = (v_half - v_full) / (pow(2.0, 4) - 1.0);
        row.OLP_S = std::abs(S);

        // Контроль погрешности
        if (std::abs(S) > tolerance) {
            step /= 2.0;
            row.divisions += 1;
            continue;
        } else if (std::abs(S) < tolerance / pow(2, 5)) {
            step *= 2.0;
            row.doublings += 1;
        }

        // Присваиваем V_i_hat результат с половинным шагом
        row.V_i_hat = v_half;
        row.V_diff = std::abs(v_half - v_full);

        // Обновляем для следующего шага
        x = x_full;
        v = v_full;

        row.X_i = x;
        row.V_i = v;

        double U_i = solution(x, C);
        row.U_i = U_i;

        row.U_V_diff = std::abs(U_i - v);

        results.append(row);
        series_ui->append(x, U_i);
        series_vi->append(x, v);
    }

    m_results = results;
}

QVector<DataRow> RungeKutt::TestTaskModel::getResults()
{
    return m_results;
}

}
