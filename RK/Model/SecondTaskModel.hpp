#pragma once

#include "TestTaskModel.hpp"

namespace RungeKutt
{
class SecondTaskModel : public RungeKutt::TestTaskModel

{
    Q_OBJECT
public:
    explicit SecondTaskModel(double A,
                        double B,
                        double Step,
                        int MaxSteps,
                        double Eps,
                        double BoundEps,
                        double StartU,
                        double a,
                        double b,
                        double c,
                        QObject *parent = nullptr);


    void methodFor2(double &X, std::vector<double> &V, double STEP);
    void runRK4For2(double x, std::vector<double> v,
                    QtCharts::QLineSeries *series_v0,
                    QtCharts::QLineSeries *series_v1,
                    QtCharts::QLineSeries *series_vv);
    void runRK4WithAdaptiveStepFor2(double x, std::vector<double> v,
                                    QtCharts::QLineSeries *series_v0,
                                    QtCharts::QLineSeries *series_v1,
                                    QtCharts::QLineSeries *series_vv);


private:
    std::vector<double> f2(double x, std::vector<double>& v);
};
}
