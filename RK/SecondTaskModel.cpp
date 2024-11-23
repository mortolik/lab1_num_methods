#include "SecondTaskModel.hpp"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <cmath>

using namespace QtCharts;

namespace RungeKutt
{
SecondTaskModel::SecondTaskModel(double A,
                     double B,
                     double Step,
                     int MaxSteps,
                     double Eps,
                     double BoundEps,
                     double StartU,
                     double a,
                     double b,
                     double c,
                     QObject *parent)
    : RungeKutt::TestTaskModel(A, B, Step, MaxSteps,Eps, BoundEps, StartU, parent)
{
    m_parametres.a = a;
    m_parametres.b = b;
    m_parametres.c = c;
}





std::vector<double> SecondTaskModel::f2(double x, std::vector<double>& v)
{
    double f_1 = v[1];
    double f_2 = -(m_parametres.a * v[1] * fabs(v[1]) + m_parametres.b * v[1] + m_parametres.c * v[0]);
    //double f_2 = -m_parametres.a * sin(v[0]);
    std::vector<double>func(2);
    func[0] = f_1;
    func[1] = f_2;

    return func;
}



void SecondTaskModel::methodFor2(double &X, std::vector<double> &V, double STEP)
{
    std::vector<double> k1(2), k2(2), k3(2), k4(2), temp(2);

    k1 = f2(X, V);

    temp = {V[0] + (STEP / 2.0) * k1[0], V[1] + (STEP / 2.0) * k1[1]};
    k2 = f2(X + STEP / 2.0, temp);

    temp = {V[0] + (STEP / 2.0) * k2[0], V[1] + (STEP / 2.0) * k2[1]};
    k3 = f2(X + STEP / 2.0, temp);

    temp = {V[0] + STEP * k3[0], V[1] + STEP * k3[1]};
    k4 = f2(X + STEP, temp);

    V[0] += STEP * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]) / 6.0;
    V[1] += STEP * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1]) / 6.0;
    X += STEP;
}

void SecondTaskModel::runRK4For2(
    double x,
    std::vector<double> v,
    QtCharts::QLineSeries *series_v0,
    QtCharts::QLineSeries *series_v1,
    QtCharts::QLineSeries *series_vv,
    QtCharts::QLineSeries *series_v_derivative
    ) {
    // Очищаем перед началом расчетов
    series_v0->clear();
    series_v1->clear();
    series_vv->clear();
    series_v_derivative->clear();

    // Заполняем начальные точки
    series_v0->append(x, v[0]);
    series_v1->append(x, v[1]);
    series_vv->append(v[0], v[1]);
    series_v_derivative->append(x, v[1]);

    QVector<RungeKutt::DataRow> results;

    for (int i = 1; i <= m_parametres.MAX_STEPS; ++i) {
        // Проверяем, нет ли переполнений или ошибок
        if (std::isinf(v[0]) || std::isnan(v[0]) || std::isinf(v[1]) || std::isnan(v[1])) {
            referenceInfo.IS_INF = true;
            break;
        }

        // Сохраняем результаты для таблицы
        RungeKutt::DataRow row;
        row.index = i;
        row.X_i = x;
        row.V0_i = v[0];
        row.V1_i = v[1];
        row.STEP_i = m_parametres.STEP;
        results.append(row);

        // Добавляем данные для графиков
        series_v0->append(x, v[0]);         // График V(x)
        series_v1->append(x, v[1]);         // График V'(x)
        series_vv->append(v[0], v[1]);      // Фазовый портрет
        series_v_derivative->append(x, v[1]);  // Производная

        // Выполняем шаг Рунге-Кутта
        methodFor2(x, v, m_parametres.STEP);

        // Проверяем выход за границы
        if (x > m_parametres.B) break;
    }

    // Обновляем справочную информацию
    referenceInfo.iterationsCount = results.size();
    referenceInfo.distanceToBoundary = m_parametres.B - x;
    m_results = results;
}

void SecondTaskModel::runRK4WithAdaptiveStepFor2(double x, std::vector<double> v,
                                                 QtCharts::QLineSeries *series_v0,
                                                 QtCharts::QLineSeries *series_v1,
                                                 QtCharts::QLineSeries *series_vv,
                                                 QtCharts::QLineSeries *series_v_derivative)
{
    series_v0->clear();
    series_v1->clear();
    series_vv->clear();
    series_v_derivative->clear();

    series_v0->append(x, v[0]);
    series_v1->append(x, v[1]);
    series_vv->append(v[0], v[1]);
    series_v_derivative->append(x, v[1]);

    QVector<DataRow> results;
    double step = m_parametres.STEP;
    double tolerance = m_parametres.EPS;
    double x_half, x_full;
    std::vector<double> v_half, v_full;
    const double MIN_STEP_THRESHOLD = 1e-9; // Минимально допустимый шаг

    // Переменные для справочной информации
    int doublingCount = 0;
    int reductionCount = 0;
    double maxStep = step;
    double maxStepX = x;
    double minStepX = x;
    double minStep = step;
    double maxOLP = 0.0;


    // double oldX = x;
    // std::vector<double> oldV = v;  // Это к другому способу деления шага

    for (int i = 1; i <= m_parametres.MAX_STEPS; ++i) {
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
            methodFor2(x_half, v_half, step_half);
            methodFor2(x_half, v_half, step_half);

            // Один шаг с полным шагом
            x_full = x;
            v_full = v;
            methodFor2(x_full, v_full, step);

            if (std::isinf(v_full[0]) || std::isnan(v_full[0]) || std::isinf(v_full[1]) || std::isnan(v_full[1])
                || std::isinf(v_half[0]) || std::isnan(v_half[0]) || std::isinf(v_half[1]) || std::isnan(v_half[1]))
            {
                referenceInfo.IS_INF = true;
                break;
            }

            // Локальная погрешность
            double S1 = (v_half[0] - v_full[0]) / (pow(2.0, 4) - 1.0);
            double S2 = (v_half[1] - v_full[1]) / (pow(2.0, 4) - 1.0);
            double S = sqrt(S1 * S1 + S2 * S2);
            row.OLP_S = std::abs(S) * pow(2.0, 4);

            if (row.OLP_S > maxOLP) {
                maxOLP = row.OLP_S;  // Обновляем максимальное значение ОЛП
            }

            if (std::abs(S) > tolerance) {
                // Деление шага

                step /= 2.0;
                row.divisions += 1;
                reductionCount++;


                // bool FLAG_TO_EXIT = false;
                //                 // выполняем расчёт с половинным шагом, если для него погрешность снова
                // while (!FLAG_TO_EXIT) {
                //     // оказалась большой, то снова возвращаемся и делим шаг, и так пока погрешность не будет допустимой
                //     step /= 2.0;
                //     reductionCount++;
                //     row.divisions += 1;

                //     v_full = oldV;
                //     x_full = oldX;
                //     v_half = v_full;
                //     x_half = x_full;

                //     for (int i = 0; i < 2; ++i) {
                //         methodFor2(x_half, v_half, step / 2.0);
                //     }
                //     methodFor2(x_full, v_full, step);

                //     S1 = (v_half[0] - v_full[0]) / (pow(2.0, 4.0) - 1.0);
                //     S2 = (v_half[1] - v_full[1]) / (pow(2.0, 4.0) - 1.0);
                //     S = sqrt(S1 * S1 + S2 * S2);
                //     if (abs(S) <= m_parametres.EPS) {
                //         FLAG_TO_EXIT = true;
                //         m_parametres.STEP = step;
                //     }
                // }
                // validStep = true;
            } else if (std::abs(S) < m_parametres.EPS / pow(2.0, 5.0)) {
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
                methodFor2(x, v, step); // Делаем последний шаг до границы
                break;
            }
            // if (x > m_parametres.B)
            // {
            //     double overshoot = x - m_parametres.B;
            //     x -= overshoot; // Коррекция X
            //     v[0] = series_v0->pointsVector().last().y(); // Вернуться к последнему V0
            //     v[1] = series_v1->pointsVector().last().y(); // Вернуться к последнему V1
            //     break;
            // }

            // Проверка на минимальный шаг
            if (step < MIN_STEP_THRESHOLD) {
                validStep = true; // Завершаем адаптацию шага
            }

        }


        // Обновляем значения после корректного шага
        row.V0_i_hat = v_half[0];
        row.V1_i_hat = v_half[1];
        row.V0_diff = std::abs(v_half[0] - v_full[0]);
        row.V1_diff = std::abs(v_half[1] - v_full[1]);

        x = x_full;
        v = v_full;

        row.X_i = x;
        row.V0_i = v[0];
        row.V1_i = v[1];

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
        series_v0->append(x, v[0]);
        series_v1->append(x, v[1]);
        series_vv->append(v[0], v[1]);
        series_v_derivative->append(x, v[1]);

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


}
