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
        int index;            // i
        double X_i;           // X_i
        double V_i;           // V_i
        double V_i_hat;       // V_i^
        double V_diff;        // V_i - V_i^
        double OLP_S;         // ОЛП(S)
        double STEP_i;        // STEP_i
        int divisions;        // Кол-во делений
        int doublings;        // Кол-во удвоений
        double U_i;           // U_i
        double U_V_diff;      // |U_i - V_i|
    };
}
