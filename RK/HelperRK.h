#pragma once

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
    };
    struct DataRow {
        int index = 0;            // i
        double X_i = 0;           // X_i
        double V_i = 0;           // V_i
        double V_i_hat = 0;       // V_i^
        double V_diff = 0;        // V_i - V_i^
        double OLP_S = 0;         // ОЛП(S)
        double STEP_i = 0;        // STEP_i
        int divisions = 0;        // Кол-во делений
        int doublings = 0;        // Кол-во удвоений
        double U_i = 0;           // U_i
        double U_V_diff = 0;      // |U_i - V_i|
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
    };
}
