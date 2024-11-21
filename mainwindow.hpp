#pragma once
#include <QMainWindow>

QT_FORWARD_DECLARE_CLASS(QDoubleSpinBox)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)

namespace RungeKutt
{
class TestTaskModel;
class TestTaskWidget;
class FirstTaskModel;
class FirstTaskWidget;
class SecondTaskModel;
class SecondTaskWidget;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget* mainWidget;
    RungeKutt::TestTaskModel* m_testTaskModel {nullptr};
    RungeKutt::TestTaskWidget* m_testTaskWidget {nullptr};
    RungeKutt::FirstTaskModel* m_firstTaskModel {nullptr};
    RungeKutt::FirstTaskWidget* m_firstTaskWidget {nullptr};
    RungeKutt::SecondTaskModel* m_secondTaskModel {nullptr};
    RungeKutt::SecondTaskWidget* m_secondTaskWidget {nullptr};

    QWidget* createTestTaskTab();
    QWidget* createFirstTaskTab();
    QWidget* createSecondTaskTab();

    QDoubleSpinBox* createSpinBox(const QString &labelText, QHBoxLayout *layout, double min, double max, double step);
};
