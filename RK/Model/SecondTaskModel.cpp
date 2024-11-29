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
    QtCharts::QLineSeries *series_vv
    )
{
    // Очищаем перед началом расчетов
    series_v0->clear();
    series_v1->clear();
    series_vv->clear();

    // Заполняем начальные точки
    series_v0->append(x, v[0]);
    series_v1->append(x, v[1]);
    series_vv->append(v[0], v[1]);

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
                                                 QtCharts::QLineSeries *series_vv)
{
    // Очистка графиков
    series_v0->clear();
    series_v1->clear();
    series_vv->clear();

    // Добавление начальных значений
    series_v0->append(x, v[0]);
    series_v1->append(x, v[1]);
    series_vv->append(v[0], v[1]);

    QVector<DataRow> results;

    double step = m_parametres.STEP;
    double tolerance = m_parametres.EPS;
    const double MIN_STEP_THRESHOLD = 1e-9;

    double x_half, x_full;
    std::vector<double> v_half, v_full;

    int doublingCount = 0;
    int reductionCount = 0;
    double maxStep = step;
    double maxStepX = x;
    double minStepX = x;
    double minStep = step;
    double maxOLP = 0.0;

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

            x_full = x;
            v_full = v;

            // Первый расчет: два шага с половинным шагом
            double step_half = step / 2.0;
            methodFor2(x_half, v_half, step_half);
            methodFor2(x_half, v_half, step_half);

            // Второй расчет: один шаг с полным шагом
            methodFor2(x_full, v_full, step);

            // Проверка на корректность данных
            if (std::isinf(v_full[0]) || std::isnan(v_full[0]) ||
                std::isinf(v_full[1]) || std::isnan(v_full[1]) ||
                std::isinf(v_half[0]) || std::isnan(v_half[0]) ||
                std::isinf(v_half[1]) || std::isnan(v_half[1]))
            {
                referenceInfo.IS_INF = true;
                break;
            }

            // Вычисление локальной погрешности
            double S1 = (v_half[0] - v_full[0]) / (pow(2.0, 4) - 1.0);
            double S2 = (v_half[1] - v_full[1]) / (pow(2.0, 4) - 1.0);
            double S = sqrt(S1 * S1 + S2 * S2);
            row.OLP_S = std::abs(S) * pow(2.0, 4);

            if (row.OLP_S > maxOLP) {
                maxOLP = row.OLP_S;  // Обновляем максимальное значение ОЛП
            }

            // Контроль ошибки
            if (std::abs(S) > tolerance) {
                step /= 2.0;  // Уменьшение шага
                row.divisions += 1;
                reductionCount++;
                continue;     // Повторяем шаг
            } else if (std::abs(S) < tolerance / pow(2.0, 5)) {
                step = std::min(step * 2.0, m_parametres.B - x);  // Увеличиваем шаг
                row.doublings += 1;
                doublingCount++;
                validStep = true; // Шаг подходит, выходим из цикла
            }
            else
            {
                validStep = true; // Шаг подходит, выходим из цикла
            }
            if (step < MIN_STEP_THRESHOLD) {
                validStep = true; // Завершаем адаптацию шага
            }
        }
        // Обновляем значения после корректного шага
        row.V0_i_hat = v_half[0];
        row.V1_i_hat = v_half[1];
        row.V0_diff = std::abs(v_half[0] - v_full[0]);
        row.V1_diff = std::abs(v_half[1] - v_full[1]);

        // Добавление данных в графики
        x = x_full;
        v = v_full;

        // Обновляем шаги для справочной информации
        row.X_i = x;
        row.V0_i = v[0];
        row.V1_i = v[1];
        if (step > maxStep) {
            maxStep = step;
            maxStepX = x + step;
        }
        if (step < minStep) {
            minStep = step;
            minStepX = x;
        }

        series_v0->append(x, v[0]);
        series_v1->append(x, v[1]);
        series_vv->append(v[0], v[1]);

        row.X_i = x;
        row.V0_i = v[0];
        row.V1_i = v[1];

        results.append(row);

        // Проверка выхода за правую границу
        if (x + step > m_parametres.B) {
            step = m_parametres.B - x;  // Корректируем шаг
            //methodFor2(x, v, step); // Делаем последний шаг до границы
        }
        // Проверка выхода за границы
        if (x >= m_parametres.B - m_parametres.BOUND_EPS || step < MIN_STEP_THRESHOLD) {
            break;
        }

    }
    // Сохранение результатов
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
