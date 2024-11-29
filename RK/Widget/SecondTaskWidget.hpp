#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "qlabel.h"
#include "qtablewidget.h"
#include "qwidget.h"

using namespace QtCharts;

namespace RungeKutt
{
class SecondTaskModel;

class SecondTaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SecondTaskWidget(SecondTaskModel *model, QWidget *parent = nullptr);
    void setUseAdaptiveMethod(bool useAdaptiveMethod);  // Новый метод

public slots:
    void calculateResults(); // Слот для запуска расчета

private slots:
    void updateTable();        // Слот для обновления таблицы с результатами

private:
    QTableWidget *createTableWidget();      // Метод для создания виджета таблицы
    QtCharts::QChartView *createChartView(QLineSeries *series, const QString &title);
    QtCharts::QChartView *createChartViewForDerivative();
    QtCharts::QChartView *createChartViewForPhasePortrait(); // Метод для создания виджета графика
    //QtCharts::QChartView *createChartView2(); // Если раскомментить, то не запуститься, Толя хелп
    //QtCharts::QChartView *createChartView3(); // Если раскомментить, то не запуститься, Толя хелп

    SecondTaskModel *m_model;                    // Модель данных для расчетов

    bool m_useAdaptiveMethod = false;  // Поле для хранения выбора метод

    QTableWidget *m_tableWidget;               // Виджет таблицы для отображения результатов
    QtCharts::QChartView *m_chartV;         // Виджет графика  // От пупсика V(x)
    QtCharts::QChartView *m_chartVDerivative;         // Виджет графика // От пупсика V'(x)
    QtCharts::QChartView *m_chartVV;         // Виджет графика // От пупсика V'(v)
    QtCharts::QLineSeries *m_seriesV;   // Серия для графика решения //От пупсика V(x)
    QtCharts::QLineSeries *m_seriesVDerivative; // Серия для графика производной //От пупсика V'(x)
    QtCharts::QLineSeries *m_seriesVV; // Серия для графика производной //От пупсика V'(V)


    QLabel *m_iterationsLabel;
    QLabel *m_distanceLabel;
    QLabel *m_maxOLPLabel;
    QLabel *m_doublingsLabel;
    QLabel *m_reductionsLabel;
    QLabel *m_maxStepLabel;
    QLabel *m_minStepLabel;
    QLabel *m_maxErrorLabel;

    void updateReferenceInfo();
};
}
