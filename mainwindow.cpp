#include <QLabel>
#include <QTabWidget>

#include "mainwindow.hpp"
#include "RK/TestTaskModel.hpp"
#include "RK/TestTaskWidget.hpp"
#include "RK/FirstTaskModel.hpp"
#include "RK/FirstTaskWidget.hpp"
#include "RK/SecondTaskModel.hpp"
#include "RK/SecondTaskWidget.hpp"
#include "qboxlayout.h"
#include "qcheckbox.h"
#include "qpushbutton.h"
#include "qspinbox.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("mainWindow");
    setMinimumSize(800, 600);

    QWidget *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    // Создаем вкладки
    QTabWidget *tabWidget = new QTabWidget(this);

    // Вкладка TestTask
    QWidget *testTaskTab = createTestTaskTab();
    tabWidget->addTab(testTaskTab, "TestTask");

    // Вкладка FirstTask
    QWidget *firstTaskTab = createFirstTaskTab();
    tabWidget->addTab(firstTaskTab, "FirstTask");

    // Вкладка SecondTask
    QWidget *secondTaskTab = createSecondTaskTab();
    tabWidget->addTab(secondTaskTab, "SecondTask");

    mainLayout->addWidget(tabWidget);
}

QWidget* MainWindow::createTestTaskTab() {
    // Создаем контейнер для вкладки
    QWidget *testTaskTab = new QWidget(this);

    QVBoxLayout *tabLayout = new QVBoxLayout(testTaskTab);
    tabLayout->setAlignment(Qt::AlignTop);

    // Горизонтальный макет для параметров
    QHBoxLayout *paramLayout = new QHBoxLayout();
    tabLayout->addLayout(paramLayout);

    // Создание спинбоксов для параметров модели
    auto *aSpinBox = createSpinBox("A:", paramLayout, -10.0, 10.0, 1.0);
    auto *bSpinBox = createSpinBox("B:", paramLayout, -10.0, 10.0, 1.0);
    auto *stepSpinBox = createSpinBox("Step:", paramLayout, 0.0001, 1.0, 0.001);
    auto *maxStepsSpinBox = createSpinBox("Max Steps:", paramLayout, 1, 1000, 100);
    auto *epsSpinBox = createSpinBox("Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *boundEpsSpinBox = createSpinBox("Bound Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *startUSpinBox = createSpinBox("Start U:", paramLayout, -100.0, 100.0, 1.0);

    // Установка значений по умолчанию
    aSpinBox->setValue(0.0);
    bSpinBox->setValue(5.0);
    stepSpinBox->setValue(0.01);
    maxStepsSpinBox->setValue(500);
    epsSpinBox->setValue(0.001);
    boundEpsSpinBox->setValue(0.01);
    startUSpinBox->setValue(1.0);

    // Чекбокс для метода с контролем погрешности
    QCheckBox *lpCheckBox = new QCheckBox("ЛП", this);
    tabLayout->addWidget(lpCheckBox);

    // Кнопка для создания модели и запуска расчетов
    QPushButton *createButton = new QPushButton("Создать модель и обновить виджет", this);
    tabLayout->addWidget(createButton);

    connect(createButton, &QPushButton::clicked, this, [=]() {
        // Очистка старых объектов
        if (m_testTaskModel != nullptr)
            delete m_testTaskModel;
        if (m_testTaskWidget != nullptr) {
            tabLayout->removeWidget(m_testTaskWidget);
            delete m_testTaskWidget;
        }

        // Создание модели
        double A = aSpinBox->value();
        double B = bSpinBox->value();
        double Step = stepSpinBox->value();
        int MaxSteps = static_cast<int>(maxStepsSpinBox->value());
        double Eps = epsSpinBox->value();
        double BoundEps = boundEpsSpinBox->value();
        double StartU = startUSpinBox->value();

        m_testTaskModel = new RungeKutt::TestTaskModel(A, B, Step, MaxSteps, Eps, BoundEps, StartU, this);
        m_testTaskWidget = new RungeKutt::TestTaskWidget(m_testTaskModel, this);

        // Передаем состояние чекбокса в виджет
        m_testTaskWidget->setUseAdaptiveMethod(lpCheckBox->isChecked());

        tabLayout->addWidget(m_testTaskWidget, 1);
    });

    return testTaskTab;
}

QWidget* MainWindow::createFirstTaskTab() {
    // Создаем контейнер для вкладки
    QWidget *firstTaskTab = new QWidget(this);

    QVBoxLayout *tabLayout = new QVBoxLayout(firstTaskTab);
    tabLayout->setAlignment(Qt::AlignTop);

    // Горизонтальный макет для параметров
    QHBoxLayout *paramLayout = new QHBoxLayout();
    tabLayout->addLayout(paramLayout);

    // Создание спинбоксов для параметров модели
    auto *aSpinBox = createSpinBox("A:", paramLayout, -10.0, 10.0, 1.0);
    auto *bSpinBox = createSpinBox("B:", paramLayout, -10.0, 10.0, 1.0);
    auto *stepSpinBox = createSpinBox("Step:", paramLayout, 0.0001, 1.0, 0.001);
    auto *maxStepsSpinBox = createSpinBox("Max Steps:", paramLayout, 1, 1000, 100);
    auto *epsSpinBox = createSpinBox("Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *boundEpsSpinBox = createSpinBox("Bound Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *startUSpinBox = createSpinBox("Start U:", paramLayout, -100.0, 100.0, 1.0);

    // Установка значений по умолчанию
    aSpinBox->setValue(0.0);
    bSpinBox->setValue(5.0);
    stepSpinBox->setValue(0.01);
    maxStepsSpinBox->setValue(500);
    epsSpinBox->setValue(0.001);
    boundEpsSpinBox->setValue(0.01);
    startUSpinBox->setValue(1.0);

    // Чекбокс для метода с контролем погрешности
    QCheckBox *lpCheckBox = new QCheckBox("ЛП", this);
    tabLayout->addWidget(lpCheckBox);

    // Кнопка для создания модели и запуска расчетов
    QPushButton *createButton = new QPushButton("Создать модель и обновить виджет", this);
    tabLayout->addWidget(createButton);

    connect(createButton, &QPushButton::clicked, this, [=]() {
        // Очистка старых объектов
        if (m_firstTaskModel != nullptr)
            delete m_firstTaskModel;
        if (m_firstTaskWidget != nullptr) {
            tabLayout->removeWidget(m_firstTaskWidget);
            delete m_firstTaskWidget;
        }

        // Создание модели
        double A = aSpinBox->value();
        double B = bSpinBox->value();
        double Step = stepSpinBox->value();
        int MaxSteps = static_cast<int>(maxStepsSpinBox->value());
        double Eps = epsSpinBox->value();
        double BoundEps = boundEpsSpinBox->value();
        double StartU = startUSpinBox->value();

        m_firstTaskModel = new RungeKutt::FirstTaskModel(A, B, Step, MaxSteps, Eps, BoundEps, StartU, this);
        m_firstTaskWidget = new RungeKutt::FirstTaskWidget(m_firstTaskModel, this);

        // Передаем состояние чекбокса в виджет
        m_firstTaskWidget->setUseAdaptiveMethod(lpCheckBox->isChecked());

        tabLayout->addWidget(m_firstTaskWidget, 1);
    });

    return firstTaskTab;
}


QWidget* MainWindow::createSecondTaskTab() {
    // Создаем контейнер для вкладки
    QWidget *secondTaskTab = new QWidget(this);

    QVBoxLayout *tabLayout = new QVBoxLayout(secondTaskTab);
    tabLayout->setAlignment(Qt::AlignTop);

    // Горизонтальный макет для параметров
    QHBoxLayout *paramLayout = new QHBoxLayout();
    tabLayout->addLayout(paramLayout);

    // Создание спинбоксов для параметров модели
    auto *aSpinBox = createSpinBox("A:", paramLayout, -10.0, 10.0, 1.0);
    auto *bSpinBox = createSpinBox("B:", paramLayout, -10.0, 10.0, 1.0);
    auto *stepSpinBox = createSpinBox("Step:", paramLayout, 0.0001, 1.0, 0.001);
    auto *maxStepsSpinBox = createSpinBox("Max Steps:", paramLayout, 1, 1000, 100);
    auto *epsSpinBox = createSpinBox("Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *boundEpsSpinBox = createSpinBox("Bound Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *startUSpinBox = createSpinBox("Start U:", paramLayout, -100.0, 100.0, 1.0);
    auto *aParamSpinBox = createSpinBox("a:", paramLayout, -10.0, 10.0, 1.0);
    auto *bParamSpinBox = createSpinBox("b:", paramLayout, -10.0, 10.0, 1.0);
    auto *cParamSpinBox = createSpinBox("c:", paramLayout, -10.0, 10.0, 1.0);

    // Установка значений по умолчанию
    aSpinBox->setValue(0.0);
    bSpinBox->setValue(5.0);
    stepSpinBox->setValue(0.01);
    maxStepsSpinBox->setValue(500);
    epsSpinBox->setValue(0.001);
    boundEpsSpinBox->setValue(0.01);
    startUSpinBox->setValue(1.0);
    aParamSpinBox->setValue(1.0);
    bParamSpinBox->setValue(1.0);
    cParamSpinBox->setValue(1.0);

    // Чекбокс для метода с контролем погрешности
    QCheckBox *lpCheckBox = new QCheckBox("ЛП", this);
    tabLayout->addWidget(lpCheckBox);

    // Кнопка для создания модели и запуска расчетов
    QPushButton *createButton = new QPushButton("Создать модель и обновить виджет", this);
    tabLayout->addWidget(createButton);

    connect(createButton, &QPushButton::clicked, this, [=]() {
        // Очистка старых объектов
        if (m_secondTaskModel != nullptr)
            delete m_secondTaskModel;
        if (m_secondTaskWidget != nullptr) {
            tabLayout->removeWidget(m_secondTaskWidget);
            delete m_secondTaskWidget;
        }

        // Создание модели
        double A = aSpinBox->value();
        double B = bSpinBox->value();
        double Step = stepSpinBox->value();
        int MaxSteps = static_cast<int>(maxStepsSpinBox->value());
        double Eps = epsSpinBox->value();
        double BoundEps = boundEpsSpinBox->value();
        double StartU = startUSpinBox->value();
        double a = aParamSpinBox->value();
        double b = bParamSpinBox->value();
        double c = cParamSpinBox->value();

        m_secondTaskModel = new RungeKutt::SecondTaskModel(A, B, Step, MaxSteps, Eps, BoundEps, StartU, a, b, c, this);
        m_secondTaskWidget = new RungeKutt::SecondTaskWidget(m_secondTaskModel, this);

        // Передаем состояние чекбокса в виджет
        m_secondTaskWidget->setUseAdaptiveMethod(lpCheckBox->isChecked());

        tabLayout->addWidget(m_secondTaskWidget, 1);
    });

    return secondTaskTab;
}


MainWindow::~MainWindow() {
    delete m_testTaskModel;
    delete m_testTaskWidget;
    delete m_firstTaskModel;
    delete m_firstTaskWidget;
    delete m_secondTaskModel;
    delete m_secondTaskWidget;
}

QDoubleSpinBox* MainWindow::createSpinBox(const QString &labelText, QHBoxLayout *layout, double min, double max, double step) {
    QLabel *label = new QLabel(labelText, this);
    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
    spinBox->setRange(min, max);
    spinBox->setSingleStep(step);
    spinBox->setDecimals(3);
    layout->addWidget(label);
    layout->addWidget(spinBox);
    return spinBox;
}
