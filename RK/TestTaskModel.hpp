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
    void method(double &X, double &V, double STEP);
    void setA(double a);
    void setDt(double dt);
    double getA() const;
    double getDt() const;
    int getMaxSteps() const;
    QVector<DataRow> getResults();

    Parametres m_parametres;

    void runRK4(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi);
    void runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi);

    ReferenceInfo getReference() const;

private:

    double f(double x, double v);
    double dudx(double x, double v);

    double constanta(double x, double v_0);
    double solution(double x, double C);

    QVector<DataRow> m_results;

    ReferenceInfo referenceInfo;
    void updateReferenceInfo(int iterations, double distance, double maxOLP, int doublings, int reductions, double maxStep, double maxStepX, double minStepX,
                             double minStep, double maxError, double maxErrorX);

};
}

