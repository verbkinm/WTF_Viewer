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

CONFIG += c++17

SOURCES += \
    calibration/generalcalibration.cpp \
    calibration/pixelcalibration.cpp \
    eventfilter/eventfilter.cpp \
    eventfilter/fingerslide.cpp \
    export/export.cpp \
    graph/centralwidget.cpp \
    graph/chart.cpp \
    graph/chartview.cpp \
    graph/customtablemodel.cpp \
    graph/graphdialog.cpp \
    graph/panelwidget.cpp \
    graph/rangeaxis.cpp \
    settings/settingsimage.cpp \
    viewer_widget/checkFile/checkfile.cpp \
    viewer_widget/frames/frames.cpp \
    viewer_widget/frames/oneframe.cpp \
    viewer_widget/viewer/panels/clogfilterpanel/clogfilterpanel.cpp \
    viewer_widget/masksettings.cpp \
    viewer_widget/viewer/panels/clogfilterpanel/rangewidget_extended.cpp \
    viewer_widget/viewer/panels/pix_filter_panel.cpp \
    viewer_widget/table.cpp \
    viewer_widget/viewer/panels/clogfilterpanel/rangewidget.cpp \
    viewer_widget/viewer/saver.cpp \
    viewer_widget/viewer/panels/toolspanel.cpp \
    viewer_widget/viewer/viewer.cpp \
    viewer_widget/viewer/panels/viewer_button_panel.cpp \
    viewer_widget/viewer/panels/viewer_data_panel.cpp \
    viewer_widget/viewer/viewer_processor/filter_clog.cpp \
    viewer_widget/viewer/viewer_processor/viewer_clog_processor.cpp \
    viewer_widget/viewer/viewer_processor/viewer_processor.cpp \
    viewer_widget/viewer/viewer_processor/viewer_txt_processor.cpp \
    viewer_widget/viewer_widget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    calibration/generalcalibration.h \
    calibration/pixelcalibration.h \
    eventfilter/eventfilter.h \
    eventfilter/fingerslide.h \
    export/export.h \
    graph/centralwidget.h \
    graph/chart.h \
    graph/chartview.h \
    graph/customtablemodel.h \
    graph/graphdialog.h \
    graph/panelwidget.h \
    graph/rangeaxis.h \
    settings/settingsimage.h \
    viewer_widget/checkFile/checkfile.h \
    viewer_widget/frames/frames.h \
    viewer_widget/frames/oneframe.h \
    viewer_widget/viewer/panels/clogfilterpanel/clogfilterpanel.h \
    viewer_widget/masksettings.h \
    viewer_widget/viewer/panels/clogfilterpanel/rangewidget_extended.h \
    viewer_widget/viewer/panels/pix_filter_panel.h \
    viewer_widget/table.h \
    viewer_widget/viewer/panels/clogfilterpanel/rangewidget.h \
    viewer_widget/viewer/saver.h \
    viewer_widget/viewer/panels/toolspanel.h \
    viewer_widget/viewer/viewer.h \
    viewer_widget/viewer/panels/viewer_button_panel.h \
    viewer_widget/viewer/panels/viewer_data_panel.h \
    viewer_widget/viewer/viewer_processor/filter_clog.h \
    viewer_widget/viewer/viewer_processor/viewer_clog_processor.h \
    viewer_widget/viewer/viewer_processor/viewer_processor.h \
    viewer_widget/viewer/viewer_processor/viewer_txt_processor.h \
    viewer_widget/viewer_widget.h \
    mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    calibration/generalcalibration.ui \
    calibration/pixelcalibration.ui \
    export/export.ui \
    graph/graphdialog.ui \
    graph/panelwidget.ui \
    loadcalibrationfiles.ui \
    settings/settingsimage.ui \
    viewer_widget/viewer/panels/clogfilterpanel/clogfilterpanel.ui \
    viewer_widget/masksettings.ui \
    viewer_widget/viewer/panels/pix_filter_panel.ui \
    viewer_widget/table.ui \
    viewer_widget/viewer/panels/clogfilterpanel/rangewidget.ui \
    viewer_widget/viewer/panels/toolspanel.ui \
    viewer_widget/viewer/viewer.ui \
    viewer_widget/viewer/panels/viewer_button_panel.ui \
    viewer_widget/viewer/panels/viewer_data_panel.ui \
    viewer_widget/viewer_widget.ui \
    progressbar.ui

RESOURCES += \
    img.qrc

DISTFILES +=

RC_ICONS = atom.ico


