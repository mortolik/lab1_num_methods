#include "TestTaskWidget.hpp"
#include "RK/TestTaskModel.hpp"
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
        m_seriesDerivative = new QLineSeries();

        chart->addSeries(m_seriesSolution);
        chart->addSeries(m_seriesDerivative);
        chart->setTitle("График решения u(x)");

        // Настройка осей
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("X");
        chart->addAxis(axisX, Qt::AlignBottom);
        m_seriesSolution->attachAxis(axisX);
        m_seriesDerivative->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("u(x)");
        chart->addAxis(axisY, Qt::AlignLeft);
        m_seriesSolution->attachAxis(axisY);
        m_seriesDerivative->attachAxis(axisY);

        chart->legend()->hide();

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        return chartView;
    }

    void RungeKutt::TestTaskWidget::calculateResults() {
        // Запуск метода runRK4 для вычислений
        double x0 = m_model->m_parametres.A;
        double v0 = m_model->m_parametres.START_U;

        m_model->runRK4(x0, v0, m_seriesSolution, m_seriesDerivative);

        // Обновление таблицы с результатами
        updateTable();
    }

    void RungeKutt::TestTaskWidget::updateTable() {
        const auto results = m_model->getResults();
        m_tableWidget->setRowCount(results.size());

        for (int i = 0; i < results.size(); ++i) {
            const auto &row = results[i];
            m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(row.index)));
            m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(row.X_i)));
            m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(row.V_i)));
            m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(row.V_i_hat)));
            m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(row.V_diff)));
            m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(row.OLP_S)));
            m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(row.STEP_i)));
            m_tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(row.divisions)));
            m_tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(row.doublings)));
            m_tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(row.U_i)));
            m_tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(row.U_V_diff)));
        }
    }
}
