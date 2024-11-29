#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "qlabel.h"
#include "qtablewidget.h"
#include "qwidget.h"

using namespace QtCharts;

namespace RungeKutt
{
class FirstTaskModel;

class FirstTaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FirstTaskWidget(FirstTaskModel *model, QWidget *parent = nullptr);
    void setUseAdaptiveMethod(bool useAdaptiveMethod);  // Новый метод

public slots:
    void calculateResults(); // Слот для запуска расчета

private slots:
    void updateTable();        // Слот для обновления таблицы с результатами

private:
    QTableWidget *createTableWidget();      // Метод для создания виджета таблицы
    QtCharts::QChartView *createChartView(); // Метод для создания виджета графика

    FirstTaskModel *m_model;                    // Модель данных для расчетов

    bool m_useAdaptiveMethod = false;  // Поле для хранения выбора метод

    QTableWidget *m_tableWidget;               // Виджет таблицы для отображения результатов
    QtCharts::QChartView *m_chartView;         // Виджет графика
    QtCharts::QLineSeries *m_seriesSolution;   // Серия для графика решения
    QtCharts::QLineSeries *m_seriesDerivative; // Серия для графика производной


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
