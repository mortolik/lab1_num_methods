QT       += core gui
QT += core gui charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RK/Model/FirstTaskModel.cpp \
    RK/Model/SecondTaskModel.cpp \
    RK/Model/TestTaskModel.cpp \
    RK/Widget/FirstTaskWidget.cpp \
    RK/Widget/SecondTaskWidget.cpp \
    RK/Widget/TestTaskWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    HelperRK.h \
    RK/HelperRK.h \
    RK/Model/FirstTaskModel.hpp \
    RK/Model/SecondTaskModel.hpp \
    RK/Model/TestTaskModel.hpp \
    RK/Widget/FirstTaskWidget.hpp \
    RK/Widget/SecondTaskWidget.hpp \
    RK/Widget/TestTaskWidget.hpp \
    mainwindow.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
