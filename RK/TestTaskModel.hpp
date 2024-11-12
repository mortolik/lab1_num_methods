#pragma once

#include "RK/HelperRK.h"
#include <QObject>
#include <QtCharts/QLineSeries>

namespace RungeKutt
{

class TestTaskModel : public QObject
{
    Q_OBJECT
public:
    explicit TestTaskModel(double A, double B, double Step, int MaxSteps, double Eps, double BoundEps, double StartU, QObject *parent = nullptr);
    void method(double X, double V, double Step, QtCharts::QLineSeries *series_x, QtCharts::QLineSeries *series_dudx);
    void setA(double a);
    void setDt(double dt);
    double getA() const;
    double getDt() const;
    int getMaxSteps() const;
    QVector<DataRow> getResults();

    Parametres m_parametres;

    void runRK4(double x, double v, QtCharts::QLineSeries *series_x, QtCharts::QLineSeries *series_dudx);

private:

    double f(double x, double v);
    double dudx(double x, double v);

    double constanta(double x, double v_0);
    double solution(double x, double C);

    QVector<DataRow> m_results;


    double m_a;
    double m_x0;
    double m_t0;
    double m_dt;
    int m_steps;

};
}

