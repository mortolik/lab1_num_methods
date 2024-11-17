QT       += core gui
QT += core gui charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RK/FirstTaskModel.cpp \
    RK/TestTaskModel.cpp \
    RK/TestTaskWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    HelperRK.h \
    RK/FirstTaskModel.hpp \
    RK/HelperRK.h \
    RK/TestTaskModel.hpp \
    RK/TestTaskWidget.hpp \
    mainwindow.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
