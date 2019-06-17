#-------------------------------------------------
#
# Project created by QtCreator 2018-10-28T17:44:35
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WTF_Viewer
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

CONFIG += c++11

SOURCES += \
    eventfilter/eventfilter.cpp \
    eventfilter/fingerslide.cpp \
    export/export.cpp \
    graph/centralwidget.cpp \
    graph/chart.cpp \
    graph/chartview.cpp \
    graph/customtablemodel.cpp \
    graph/graphdialog.cpp \
    graph/panelwidget.cpp \
    settings/settingsimage.cpp \
    viewer/checkFile/checkfile.cpp \
    viewer/frames/frames.cpp \
    viewer/frames/oneframe.cpp \
    viewer/clogfilterpanel.cpp \
    viewer/masksettings.cpp \
    viewer/table.cpp \
    viewer/toolspanel.cpp \
    viewer/viewer.cpp \
    viewer/viewer_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    progressbar.cpp

HEADERS += \
    eventfilter/eventfilter.h \
    eventfilter/fingerslide.h \
    export/export.h \
    graph/centralwidget.h \
    graph/chart.h \
    graph/chartview.h \
    graph/customtablemodel.h \
    graph/graphdialog.h \
    graph/panelwidget.h \
    settings/settingsimage.h \
    viewer/checkFile/checkfile.h \
    viewer/frames/frames.h \
    viewer/frames/oneframe.h \
    viewer/clogfilterpanel.h \
    viewer/masksettings.h \
    viewer/table.h \
    viewer/toolspanel.h \
    viewer/viewer.h \
    viewer/viewer_widget.h \
    mainwindow.h \
    progressbar.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    export/export.ui \
    graph/graphdialog.ui \
    graph/panelwidget.ui \
    loadcalibrationfiles.ui \
    settings/settingsimage.ui \
    viewer/clogfilterpanel.ui \
    viewer/masksettings.ui \
    viewer/table.ui \
    viewer/toolspanel.ui \
    viewer/viewer.ui \
    viewer/viewer_widget.ui \
    progressbar.ui

RESOURCES += \
    img.qrc

DISTFILES +=

RC_ICONS = atom.ico


