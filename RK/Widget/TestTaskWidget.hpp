#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QVBoxLayout>
#include "TestTaskModel.hpp"
#include "qlabel.h"

using namespace QtCharts;

namespace RungeKutt {

class TestTaskWidget : public QWidget {
    Q_OBJECT

public:
    explicit TestTaskWidget(TestTaskModel *model, QWidget *parent = nullptr);
    void setUseAdaptiveMethod(bool useAdaptiveMethod);  // Новый метод

public slots:
    void calculateResults(); // Слот для запуска расчета

private slots:
    void updateTable();        // Слот для обновления таблицы с результатами

private:
    QTableWidget *createTableWidget();      // Метод для создания виджета таблицы
    QtCharts::QChartView *createChartView(); // Метод для создания виджета графика

    TestTaskModel *m_model;                    // Модель данных для расчетов

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
