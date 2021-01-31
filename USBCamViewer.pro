#-------------------------------------------------
#
# Project created by QtCreator 2020-01-31T08:08:46
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = usbcamviewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG += c++11 release

INCLUDEPATH += /usr/local/include \
    /usr/local/include/opencv4

LIBS += -L/usr/local/lib \
    -lavcodec\
    -lavutil\
    -lavformat\
    -lswscale\
    -lswresample \
    -lturbojpeg \
    -lgpujpeg \
    -lopencv_world \

SOURCES += \
    camconfig.cpp \
    camdialog.cpp \
    capturethread.cpp \
#    defog.cpp \
    darkchannel.cpp \
    dehaze.cpp \
    displaythread.cpp \
    fastguidedfilter.cpp \
    glcanvas.cpp \
    guidedfilter.cpp \
    imagebrowserdlg.cpp \
    imagebuffer.cpp \
    imagebutton.cpp \
    imagereader.cpp \
    imageviewerdlg.cpp \
    jpegdecode.cpp \
    main.cpp \
    mainwindow.cpp \
    photothread.cpp \
    processthread.cpp \
    replaycanvas.cpp \
    settingdialog.cpp \
    v4l2capture.cpp \
    videoplayer.cpp \
    videothread.cpp \

HEADERS += \
    camconfig.h \
    camdialog.h \
    capturethread.h \
#    defog.h \
    darkchannel.h \
    dehaze.h \
    displaythread.h \
    fastguidedfilter.h \
    glcanvas.h \
    global.h \
    guidedfilter.h \
    imagebrowserdlg.h \
    imagebuffer.h \
    imagebutton.h \
    imagereader.h \
    imageviewerdlg.h \
    jpegdecode.h \
    mainwindow.h \
    photothread.h \
    processthread.h \
    replaycanvas.h \
    settingdialog.h \
    v4l2capture.h \
    videoplayer.h \
    videothread.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    usbcam.qrc

FORMS += \
    settingdialog.ui
