#include <QLabel>

#include "mainwindow.hpp"
#include "RK/TestTaskModel.hpp"
#include "RK/TestTaskWidget.hpp"
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qspinbox.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("mainWindow");
    setMinimumSize(800, 600);

    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    QHBoxLayout *paramLayout = new QHBoxLayout();
    mainLayout->addLayout(paramLayout);

    // Создание спинбоксов для параметров модели
    auto *aSpinBox = createSpinBox("A:", paramLayout, -10.0, 10.0, 1.0);
    auto *bSpinBox = createSpinBox("B:", paramLayout, -10.0, 10.0, 1.0);
    auto *stepSpinBox = createSpinBox("Step:", paramLayout, 0.0001, 1.0, 0.001);
    auto *maxStepsSpinBox = createSpinBox("Max Steps:", paramLayout, 1, 1000, 100);
    auto *epsSpinBox = createSpinBox("Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *boundEpsSpinBox = createSpinBox("Bound Eps:", paramLayout, 0.0001, 1.0, 0.001);
    auto *startUSpinBox = createSpinBox("Start U:", paramLayout, -100.0, 100.0, 1.0);


    aSpinBox->setValue(0.0);
    bSpinBox->setValue(5.0);
    stepSpinBox->setValue(0.01);
    maxStepsSpinBox->setValue(500.0);
    epsSpinBox->setValue(0.001);
    boundEpsSpinBox->setValue(0.01);
    startUSpinBox->setValue(1.0);
    // Кнопка для создания модели и запуска расчетов
    QPushButton *createButton = new QPushButton("Создать модель и обновить виджет", this);
    mainLayout->addWidget(createButton);

    // Соединение кнопки с логикой создания модели и виджета
    connect(createButton, &QPushButton::clicked, this, [=]() {
        // Очистка предыдущей модели и виджета
        if (m_testTaskModel != nullptr)
            delete m_testTaskModel;
        if (m_testTaskWidget != nullptr)
            delete m_testTaskWidget;

        // Создание модели с полученными параметрами
        double A = aSpinBox->value();
        double B = bSpinBox->value();
        double Step = stepSpinBox->value();
        int MaxSteps = static_cast<int>(maxStepsSpinBox->value());
        double Eps = epsSpinBox->value();
        double BoundEps = boundEpsSpinBox->value();
        double StartU = startUSpinBox->value();

        m_testTaskModel = new RungeKutt::TestTaskModel(A, B, Step, MaxSteps, Eps, BoundEps, StartU, this);

        // Запуск расчетов
        //m_testTaskModel->runRK4(A, StartU, nullptr, nullptr); // Вызовите расчеты перед передачей модели в виджет

        // Создание виджета и передача ему модели
        m_testTaskWidget = new RungeKutt::TestTaskWidget(m_testTaskModel, this);
        mainLayout->addWidget(m_testTaskWidget);
    });
}

MainWindow::~MainWindow() {
    delete m_testTaskModel;
    delete m_testTaskWidget;
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
