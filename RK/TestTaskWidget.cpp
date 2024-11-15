#include "TestTaskWidget.hpp"
#include "RK/TestTaskModel.hpp"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDebug>


using namespace QtCharts;

namespace RungeKutt
{
RungeKutt::TestTaskWidget::TestTaskWidget(TestTaskModel *model, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QTabWidget *tabWidget = new QTabWidget(this);

    m_tableWidget = createTableWidget();
    tabWidget->addTab(m_tableWidget, "Таблица");

    m_chartView = createChartView();
    tabWidget->addTab(m_chartView, "График");

    QWidget *referenceWidget = new QWidget(this);
    QVBoxLayout *referenceLayout = new QVBoxLayout(referenceWidget);

    m_iterationsLabel = new QLabel(referenceWidget);
    m_distanceLabel = new QLabel(referenceWidget);
    m_maxOLPLabel = new QLabel(referenceWidget);
    m_doublingsLabel = new QLabel(referenceWidget);
    m_reductionsLabel = new QLabel(referenceWidget);
    m_maxStepLabel = new QLabel(referenceWidget);
    m_minStepLabel = new QLabel(referenceWidget);
    m_maxErrorLabel = new QLabel(referenceWidget);

    referenceLayout->addWidget(m_iterationsLabel);
    referenceLayout->addWidget(m_distanceLabel);
    referenceLayout->addWidget(m_maxOLPLabel);
    referenceLayout->addWidget(m_doublingsLabel);
    referenceLayout->addWidget(m_reductionsLabel);
    referenceLayout->addWidget(m_maxStepLabel);
    referenceLayout->addWidget(m_minStepLabel);
    referenceLayout->addWidget(m_maxErrorLabel);

    referenceWidget->setLayout(referenceLayout);
    tabWidget->addTab(referenceWidget, "Справка");

    mainLayout->addWidget(tabWidget);

    QPushButton *calculateButton = new QPushButton("Запустить расчет", this);
    connect(calculateButton, &QPushButton::clicked, this, &TestTaskWidget::calculateResults);
    mainLayout->addWidget(calculateButton);

    setLayout(mainLayout);
}

    QTableWidget *RungeKutt::TestTaskWidget::createTableWidget() {
        QTableWidget *tableWidget = new QTableWidget(0, 11, this);

        // Заголовки столбцов
        tableWidget->setHorizontalHeaderLabels({
            "i", "X_i", "V_i", "V_i^", "V_i - V_i^", "ОЛП(S)",
            "STEP_i", "Кол-во делений", "Кол-во удвоений", "U_i", "|U_i - V_i|"
        });

        return tableWidget;
    }

    QChartView *RungeKutt::TestTaskWidget::createChartView() {
        QChart *chart = new QChart();
        m_seriesSolution = new QLineSeries();
        m_seriesSolution->setName("Точное решение U(x)");
        QPen solutionPen(Qt::red);
        solutionPen.setWidth(3);  // Увеличиваем толщину линии, чтобы было видно совпадение линий
        m_seriesSolution->setPen(solutionPen);

        m_seriesDerivative = new QLineSeries();
        m_seriesDerivative->setName("Численное решение V(x)");


        chart->addSeries(m_seriesSolution);
        chart->addSeries(m_seriesDerivative);
        chart->setTitle("График решения u(x)");

        // Автоматическое создание осей с учетом данных
        chart->createDefaultAxes();

        chart->legend()->setVisible(true);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        return chartView;
    }

    void TestTaskWidget::updateReferenceInfo()
    {
        auto reference = m_model->getReference();
        m_iterationsLabel->setText("Итерации: " + QString::number(reference.iterationsCount));
        m_distanceLabel->setText("Разница с границей: " + QString::number(reference.distanceToBoundary));
        m_maxOLPLabel->setText("Макс ОЛП: " + QString::number(reference.maxOLP));
        m_doublingsLabel->setText("Удвоения: " + QString::number(reference.stepDoublingCount));
        m_reductionsLabel->setText("Деления: " + QString::number(reference.stepReductionCount));
        m_maxStepLabel->setText("Макс шаг: " + QString::number(reference.maxStep) + " при X = " + QString::number(reference.maxErrorX));
        m_minStepLabel->setText("Мин шаг: " + QString::number(reference.minStep));
        m_maxErrorLabel->setText("Макс ошибка: " + QString::number(reference.maxError));
    }



    void RungeKutt::TestTaskWidget::setUseAdaptiveMethod(bool useAdaptiveMethod) {
        m_useAdaptiveMethod = useAdaptiveMethod;
    }

    void RungeKutt::TestTaskWidget::calculateResults() {
        // Запуск нужного метода в зависимости от флага
        double x0 = m_model->m_parametres.A;
        double v0 = m_model->m_parametres.START_U;

        if (m_useAdaptiveMethod) {
            m_model->runRK4WithAdaptiveStep(x0, v0, m_seriesSolution, m_seriesDerivative);
        } else {
            m_model->runRK4(x0, v0, m_seriesSolution, m_seriesDerivative);
        }

        m_chartView->chart()->axisX()->setRange(m_model->m_parametres.A, m_model->m_parametres.B);

        // Обновление диапазонов осей и построение графиков
        double minY = std::numeric_limits<double>::max();
        double maxY = std::numeric_limits<double>::lowest();

        for (const auto &point : m_seriesSolution->points()) {
            minY = std::min(minY, point.y());
            maxY = std::max(maxY, point.y());
        }
        for (const auto &point : m_seriesDerivative->points()) {
            minY = std::min(minY, point.y());
            maxY = std::max(maxY, point.y());
        }

        m_chartView->chart()->axisY()->setRange(minY, maxY);
        updateTable();
        updateReferenceInfo();  // Обновление справочной информации
    }


    void TestTaskWidget::updateTable() {
        const auto results = m_model->getResults();
        m_tableWidget->setRowCount(results.size());

        for (int i = 0; i < results.size(); ++i) {
            const auto &row = results[i];

            // Заполняем данные в каждой ячейке строки таблицы
            m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(row.index)));                // Номер шага
            m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(row.X_i, 'g', 10)));         // X_i
            m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(row.V_i, 'g', 10)));         // V_i
            m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(row.V_i_hat, 'g', 10)));     // V_i^ (если используется контроль)
            m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(row.V_diff, 'g', 10)));      // V_i - V_i^
            m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(row.OLP_S, 'g', 10)));       // ОЛП(S) - локальная погрешность
            m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(row.STEP_i, 'g', 10)));      // Шаг интегрирования
            m_tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(row.divisions)));            // Количество делений шага
            m_tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(row.doublings)));            // Количество удвоений шага
            m_tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(row.U_i, 'g', 10)));         // Точное решение U_i
            m_tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(row.U_V_diff, 'g', 10)));   // |U_i - V_i| - разница с точным решением
        }
    }



}
