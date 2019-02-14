#-------------------------------------------------
#
# Project created by QtCreator 2019-02-13T13:25:46
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Find_live2
TEMPLATE = app

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


win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_aruco344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_highgui344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_img_hash344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_imgcodecs344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_imgproc344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_line_descriptor344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_ml344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_objdetect344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_optflow344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_phase_unwrapping344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_photo344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_plot344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_reg344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_rgbd344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_saliency344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_shape344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_stereo344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_stitching344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_structured_light344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_superres344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_surface_matching344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_text344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_tracking344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_video344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_videoio344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_videostab344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_xfeatures2d344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_ximgproc344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_xobjdetect344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_xphoto344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_aruco344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_bgsegm344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_bioinspired344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_calib3d344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_ccalib344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_core344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_datasets344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_dnn_objdetect344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_dnn344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_dpm344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_face344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_features2d344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_flann344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_fuzzy344.dll
win32: LIBS += -L$$PWD/../../opencv-3.4.4/build/lib/ -llibopencv_hfs344.dll

INCLUDEPATH += $$PWD/../../opencv-3.4.4/build/include
DEPENDPATH += $$PWD/../../opencv-3.4.4/build/include

RC_FILE = logo.rc

RESOURCES += other/qss.qrc \
    other/icon.qrc
