#-------------------------------------------------
#
# Project created by QtCreator 2019-02-13T13:25:46
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Find_live2
TEMPLATE = app

RC_FILE = logo.rc

RESOURCES += other/qss.qrc \
    other/icon.qrc

#MOC_DIR             = temp/moc
#RCC_DIR             = temp/rcc
#UI_DIR              = temp/ui
#OBJECTS_DIR         = temp/obj
#DESTDIR             = $$PWD/../bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui


INCLUDEPATH += D:/opencv-3.4.4/build/install/include \

# 如果opencv的路径没有添加到系统变量 添加这个路径没有用 还得把dll库文件拷贝到exe对应的文件夹下
LIBS += -L D:\opencv-3.4.4\build\bin\libopencv_*.dll
LIBS += -L D:\opencv-3.4.4\build\bin\opencv_ffmpeg344.dll

