#pragma once

#include "TestTaskModel.hpp"
namespace RungeKutt
{
class FirstTask : public RungeKutt::TestTaskModel
{
    Q_OBJECT
public:
    explicit FirstTask(double A,
                       double B,
                       double Step,
                       int MaxSteps,
                       double Eps,
                       double BoundEps,
                       double StartU,
                       QObject *parent = nullptr);

    void runRK4(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi) override;
    virtual void runRK4WithAdaptiveStep(double x, double v, QtCharts::QLineSeries *series_ui, QtCharts::QLineSeries *series_vi) override;

private:
    double f(double x, double v) override;
};
}
