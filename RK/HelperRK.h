#pragma once

#include "qchar.h"
#include "qobject.h"
namespace RungeKutt
{
    struct Parametres
    {
        double A;
        double B;
        double STEP;
        int MAX_STEPS;
        double EPS;
        double BOUND_EPS;
        double START_U;
        double a;
        double b;
        double c;
    };
    struct DataRow {
        int index = 0;            // i
        double X_i = 0;           // X_i
        double V_i = 0;           // V_i
        double V_i_hat = 0;       // V_i^
        double V_diff = 0;        // V_i - V_i^
        double OLP_S = 0;         // ОЛП(S)
        QString OLP_S1 = 0;
        QString V_i_display = 0;           // V_i
        QString V_i_hat_display = 0;       // V_i^
        QString V_diff_display = 0;        // V_i - V_i^
        double STEP_i = 0;        // STEP_i
        int divisions = 0;        // Кол-во делений
        int doublings = 0;        // Кол-во удвоений
        double U_i = 0;           // U_i
        double U_V_diff = 0;      // |U_i - V_i|

        // Для второй задачи
        double V0_i = 0;          // V0_i
        double V1_i = 0;          // V1_i
        double V0_i_hat = 0;      // V0_i^
        double V1_i_hat = 0;      // V1_i^
        double V0_diff = 0;       // V0_i - V0_i^
        double V1_diff = 0;       // V1_i - V1_i^
    };
    struct ReferenceInfo {
        int iterationsCount;
        double distanceToBoundary;
        double maxOLP;
        int stepDoublingCount;
        int stepReductionCount;
        double maxStep;
        double maxStepX;
        double minStepX;
        double minStep;
        double maxError;
        double maxErrorX;

        bool IS_INF = false;
    };
}
