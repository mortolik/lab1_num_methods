#include "SecondTaskWidget.hpp"
#include "RK/SecondTaskModel.hpp"
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

SecondTaskWidget::SecondTaskWidget(SecondTaskModel *model, QWidget *parent)
    : QWidget(parent), m_model(model) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаем виджет таблицы и графика
    QTabWidget *tabWidget = new QTabWidget(this);
    m_tableWidget = createTableWidget();
    tabWidget->addTab(m_tableWidget, "Таблица");

    m_chartV = createChartView();
    tabWidget->addTab(m_chartV, "График V(x)");

    m_chartVDerivative = createChartView();
    tabWidget->addTab(m_chartVDerivative, "График V'(x)");

    m_chartVV = createChartView();
    tabWidget->addTab(m_chartVV, "График V'(V)");

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
    connect(calculateButton, &QPushButton::clicked, this, &SecondTaskWidget::calculateResults);
    mainLayout->addWidget(calculateButton);

    setLayout(mainLayout);
}

void SecondTaskWidget::setUseAdaptiveMethod(bool useAdaptiveMethod)
{
    m_useAdaptiveMethod = useAdaptiveMethod;
}

QTableWidget *SecondTaskWidget::createTableWidget() {
    QTableWidget *tableWidget = new QTableWidget(0, 12, this);

    // Заголовки столбцов без `U_i` и `|U_i - V_i|`
    tableWidget->setHorizontalHeaderLabels({
        "i", "X_i", "V0_i", "V1_i", "V0_i^", "V1_i^", "V0_i - V0_i^", "V1_i - V1_i^", "ОЛП(S)",
        "STEP_i", "Кол-во делений", "Кол-во удвоений"
    });

    return tableWidget;
}

QChartView *SecondTaskWidget::createChartView() {
    QChart *chart = new QChart();
    m_seriesV = new QLineSeries();
    m_seriesVDerivative = new QLineSeries();
    m_seriesVV = new QLineSeries();

    m_seriesV->setName("Численное решение V(x)");
    chart->addSeries(m_seriesV);
    chart->setTitle("График решения V(x)");

    m_seriesVDerivative->setName("График первой производной"); // ??????
    chart->addSeries(m_seriesVDerivative);
    chart->setTitle("График решения V(x)");

    m_seriesVV->setName("Фазовый портрет"); // ??????
    chart->addSeries(m_seriesVV);
    chart->setTitle("График решения V(x)");

    chart->createDefaultAxes();
    chart->legend()->setVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}


/*QChartView *SecondTaskWidget::createChartView2() {
    QChart *chart2 = new QChart();
    m_seriesVDerivative = new QLineSeries();

    m_seriesVDerivative->setName("График первой производной"); // ??????
    chart2->addSeries(m_seriesVDerivative);
    chart2->setTitle("График решения V(x)");

    chart2->createDefaultAxes();
    chart2->legend()->setVisible(true);

    QChartView *chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);

    return chartView2;
}



QChartView *SecondTaskWidget::createChartView3() {
    QChart *chart3 = new QChart();
    m_seriesVV = new QLineSeries();

    m_seriesVV->setName("Фазовый портрет"); // ??????
    chart3->addSeries(m_seriesVDerivative);
    chart3->setTitle("График решения V(x)");

    chart3->createDefaultAxes();
    chart3->legend()->setVisible(true);

    QChartView *chartView3 = new QChartView(chart3);
    chartView3->setRenderHint(QPainter::Antialiasing);

    return chartView3;
}*/

void SecondTaskWidget::updateReferenceInfo() {
    auto reference = m_model->getReference();
    m_iterationsLabel->setText("Итерации: " + QString::number(reference.iterationsCount));
    m_distanceLabel->setText("Разница с границей: " + QString::number(reference.distanceToBoundary));
    m_maxOLPLabel->setText("Макс ОЛП: " + QString::number(reference.maxOLP));
    m_doublingsLabel->setText("Удвоения: " + QString::number(reference.stepDoublingCount));
    m_reductionsLabel->setText("Деления: " + QString::number(reference.stepReductionCount));
    m_maxStepLabel->setText("Макс шаг: " + QString::number(reference.maxStep) + " при X = " + QString::number(reference.maxStepX));
    m_minStepLabel->setText("Мин шаг: " + QString::number(reference.minStep) + " при X = " + QString::number(reference.minStepX));
}

void SecondTaskWidget::calculateResults() {
    // Запуск нужного метода в зависимости от флага
    double x0 = m_model->m_parametres.A;
    std::vector<double> v0 = {m_model->m_parametres.START_U, 0.0};

    if (m_useAdaptiveMethod) {
        m_model->runRK4WithAdaptiveStepFor2(x0, v0, m_seriesV, m_seriesVDerivative, m_seriesVV);
    } else {
        m_model->runRK4For2(x0, v0, m_seriesV, m_seriesVDerivative, m_seriesVV);
    }

    // Определение диапазона по оси X
    double maxX = m_model->m_parametres.A;
    for (const auto &point : m_seriesV->points()) {
        maxX = std::max(maxX, point.x());
    }
    m_chartV->chart()->axisX()->setRange(m_model->m_parametres.A, maxX);

    for (const auto &point : m_seriesVDerivative->points()) {
        maxX = std::max(maxX, point.x());
    }
    m_chartVDerivative->chart()->axisX()->setRange(m_model->m_parametres.A, maxX);

    for (const auto &point : m_seriesVV->points()) {
        maxX = std::max(maxX, point.x());
    }
    m_chartVV->chart()->axisX()->setRange(m_model->m_parametres.A, maxX);

    // Определение диапазона по оси Y
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    for (const auto &point : m_seriesV->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }
    m_chartV->chart()->axisX()->setRange(m_model->m_parametres.A, maxX);

    for (const auto &point : m_seriesVDerivative->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }
    m_chartVDerivative->chart()->axisX()->setRange(m_model->m_parametres.A, maxX);

    for (const auto &point : m_seriesVV->points()) {
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }
    m_chartVV->chart()->axisY()->setRange(minY, maxY);

    // Обновление таблицы и справочной информации
    updateTable();
    updateReferenceInfo();
}


void SecondTaskWidget::updateTable() {
    const auto results = m_model->getResults();
    m_tableWidget->setRowCount(results.size());

    for (int i = 0; i < results.size(); ++i) {
        const auto &row = results[i];
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(row.index)));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(row.X_i, 'g', 10)));
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(row.V0_i, 'g', 10)));
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(row.V1_i, 'g', 10)));
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(row.V0_i_hat, 'g', 10)));
        m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(row.V1_i_hat, 'g', 10)));
        m_tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(row.V0_diff, 'g', 10)));
        m_tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(row.V1_diff, 'g', 10)));
        m_tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(row.OLP_S, 'g', 10)));
        m_tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(row.STEP_i, 'g', 10)));
        m_tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(row.divisions)));
        m_tableWidget->setItem(i, 11, new QTableWidgetItem(QString::number(row.doublings)));
    }
}

}  // namespace RungeKutt
