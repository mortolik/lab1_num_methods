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

        // Создание вкладок
        QTabWidget *tabWidget = new QTabWidget(this);

        // Вкладка с таблицей
        m_tableWidget = createTableWidget();
        tabWidget->addTab(m_tableWidget, "Таблица");

        // Вкладка с графиком
        m_chartView = createChartView();
        tabWidget->addTab(m_chartView, "График");

        mainLayout->addWidget(tabWidget);

        // Кнопка для запуска расчетов
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



    void RungeKutt::TestTaskWidget::calculateResults() {
        // Запуск метода runRK4 для вычислений
        double x0 = m_model->m_parametres.A;
        double v0 = m_model->m_parametres.START_U;

        m_model->runRK4(x0, v0, m_seriesSolution, m_seriesDerivative);

        // Обновление диапазона оси X по значению A и B
        m_chartView->chart()->axisX()->setRange(m_model->m_parametres.A, m_model->m_parametres.B);

        // Определяем минимальное и максимальное значения для оси Y
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

        // Устанавливаем диапазон для оси Y
        m_chartView->chart()->axisY()->setRange(minY, maxY);

        // Обновляем таблицу с результатами
        updateTable();
    }


    void TestTaskWidget::updateTable() {
        const auto results = m_model->getResults();
        m_tableWidget->setRowCount(results.size());

        for (int i = 0; i < results.size(); ++i) {
            const auto &row = results[i];

            // Используем режим 'g' с 10 знаками, чтобы убрать лишние нули и сохранить точность
            m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(row.index)));
            m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(row.X_i, 'g', 10)));  // X_i
            m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(row.V_i, 'g', 10)));  // V_i
            m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(row.V_i_hat, 'g', 10)));
            m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(row.V_diff, 'g', 10)));
            m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(row.OLP_S, 'g', 10)));
            m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(row.STEP_i, 'g', 10)));
            m_tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(row.divisions)));
            m_tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(row.doublings)));
            m_tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(row.U_i, 'g', 10)));  // U_i
            m_tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(row.U_V_diff, 'g', 10)));  // |U_i - V_i|
        }
    }


}
