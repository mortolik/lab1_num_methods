#include "FirstTaskWidget.hpp"
#include "RK/FirstTaskModel.hpp"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QTableWidget>

using namespace QtCharts;

namespace RungeKutt {

FirstTaskWidget::FirstTaskWidget(FirstTaskModel *model, QWidget *parent)
    : QWidget(parent), m_model(model) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаем виджет таблицы и графика
    QTabWidget *tabWidget = new QTabWidget(this);
    m_tableWidget = createTableWidget();
    tabWidget->addTab(m_tableWidget, "Таблица");

    m_chartView = createChartView();
    tabWidget->addTab(m_chartView, "График");

    // Создаем referenceWidget
    QWidget *referenceWidget = new QWidget(this);
    QVBoxLayout *referenceLayout = new QVBoxLayout(referenceWidget);
    referenceLayout->setSpacing(3);

    QLabel *header = new QLabel("<b>Справка</b>");
    referenceLayout->addWidget(header);

    m_iterationsLabel = new QLabel(referenceWidget);
    m_distanceLabel = new QLabel(referenceWidget);
    m_maxOLPLabel = new QLabel(referenceWidget);
    m_doublingsLabel = new QLabel(referenceWidget);
    m_reductionsLabel = new QLabel(referenceWidget);
    m_maxStepLabel = new QLabel(referenceWidget);
    m_minStepLabel = new QLabel(referenceWidget);

    referenceLayout->addWidget(m_iterationsLabel);
    referenceLayout->addWidget(m_distanceLabel);
    referenceLayout->addWidget(m_maxOLPLabel);
    referenceLayout->addWidget(m_doublingsLabel);
    referenceLayout->addWidget(m_reductionsLabel);
    referenceLayout->addWidget(m_maxStepLabel);
    referenceLayout->addWidget(m_minStepLabel);

    referenceLayout->setAlignment(Qt::AlignTop);
    referenceWidget->setLayout(referenceLayout);

    QFrame *referenceFrame = new QFrame();
    referenceFrame->setFrameShape(QFrame::Box);
    referenceFrame->setFrameShadow(QFrame::Sunken);
    referenceFrame->setLayout(referenceLayout);
    referenceFrame->setFixedWidth(230);

    // Создаем горизонтальный layout для таблицы и справки
    QHBoxLayout *tabLayout = new QHBoxLayout();
    tabLayout->addWidget(referenceFrame);
    tabLayout->addWidget(tabWidget);

    mainLayout->addLayout(tabLayout);

    // Кнопка запуска расчета
    QPushButton *calculateButton = new QPushButton("Запустить расчет", this);
    connect(calculateButton, &QPushButton::clicked, this, &FirstTaskWidget::calculateResults);
    mainLayout->addWidget(calculateButton);

    setLayout(mainLayout);
}

QTableWidget *FirstTaskWidget::createTableWidget() {
    QTableWidget *tableWidget = new QTableWidget(0, 9, this);

    // Заголовки столбцов без `U_i` и `|U_i - V_i|`
    tableWidget->setHorizontalHeaderLabels({
        "i", "X_i", "V_i", "V_i^", "V_i - V_i^", "ОЛП(S)",
        "STEP_i", "Кол-во делений", "Кол-во удвоений"
    });

    return tableWidget;
}

QChartView *FirstTaskWidget::createChartView() {
    QChart *chart = new QChart();
    m_seriesSolution = new QLineSeries();
    m_seriesSolution->setName("Численное решение V(x)");
    m_seriesDerivative = new QLineSeries();
    m_seriesDerivative->setName("Контроль V^");

    chart->addSeries(m_seriesSolution);
    chart->addSeries(m_seriesDerivative);
    chart->setTitle("График решения V(x)");

    chart->createDefaultAxes();
    chart->legend()->setVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void FirstTaskWidget::updateReferenceInfo() {
    auto reference = m_model->getReference();
    m_iterationsLabel->setText("Итерации: " + QString::number(reference.iterationsCount));
    m_distanceLabel->setText("Разница с границей: " + QString::number(reference.distanceToBoundary));
    m_maxOLPLabel->setText("Макс ОЛП: " + QString::number(reference.maxOLP));
    m_doublingsLabel->setText("Удвоения: " + QString::number(reference.stepDoublingCount));
    m_reductionsLabel->setText("Деления: " + QString::number(reference.stepReductionCount));
    m_maxStepLabel->setText("Макс шаг: " + QString::number(reference.maxStep) + " при X = " + QString::number(reference.maxStepX));
    m_minStepLabel->setText("Мин шаг: " + QString::number(reference.minStep) + " при X = " + QString::number(reference.minStepX));
}

void FirstTaskWidget::calculateResults() {
    double x0 = m_model->m_parametres.A;
    double v0 = m_model->m_parametres.START_U;

    m_model->runRK4WithAdaptiveStep(x0, v0, m_seriesSolution, m_seriesDerivative);
    m_chartView->chart()->axisX()->setRange(m_model->m_parametres.A, m_model->m_parametres.B);

    updateTable();
    updateReferenceInfo();
}

void FirstTaskWidget::updateTable() {
    const auto results = m_model->getResults();
    m_tableWidget->setRowCount(results.size());

    for (int i = 0; i < results.size(); ++i) {
        const auto &row = results[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(row.index)));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(row.X_i, 'g', 10)));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(row.V_i, 'g', 10)));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(row.V_i_hat, 'g', 10)));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(row.V_diff, 'g', 10)));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(row.OLP_S, 'g', 10)));
        m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(row.STEP_i, 'g', 10)));
        m_tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(row.divisions)));
        m_tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(row.doublings)));
    }
}

}  // namespace RungeKutt
