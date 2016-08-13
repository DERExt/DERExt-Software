#-------------------------------------------------
#
# Project created by QtCreator 2015-01-06T20:40:55
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DERExt
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    attsimple.cpp \
    attcomp.cpp \
    entity.cpp \
    entityitem.cpp \
    error.cpp \
    attdialog.cpp \
    attdialogadd.cpp \
    attdialogedit.cpp \
    graphicview.cpp \
    removedialog.cpp \
    sqldialog.cpp \
    editdialog.cpp \
    addeditbutton.cpp \
    relationshipitem.cpp \
    relationship.cpp \
    rbinaryitem.cpp \
    rbinary.cpp \
    relationshipdialog.cpp \
    runaryitem.cpp \
    weakentity.cpp \
    entitydialog.cpp \
    rternaryitem.cpp \
    rternary.cpp \
    cardinality.cpp \
    hierarchydialog.cpp \
    hierarchy.cpp \
    hierarchyitem.cpp

HEADERS  += mainwindow.h \
    attribute.h \
    attcomp.h \
    attsimple.h \
    entity.h \
    entityitem.h \
    igraphicitem.h \
    error.h \
    attdialog.h \
    attdialogadd.h \
    addpushbutton.h \
    attdialogedit.h \
    graphicview.h \
    removedialog.h \
    sqldialog.h \
    editdialog.h \
    addeditbutton.h \
    relationshipitem.h \
    relationship.h \
    rbinaryitem.h \
    rbinary.h \
    relationshipdialog.h \
    runaryitem.h \
    weakentity.h \
    entitydialog.h \
    rternaryitem.h \
    rternary.h \
    cardinality.h \
    eritem.h \
    fkcombobox.h \
    hierarchydialog.h \
    hierarchy.h \
    hierarchyitem.h

FORMS    += mainwindow.ui \
    attdialog.ui \
    attdialogadd.ui \
    attdialogedit.ui \
    removedialog.ui \
    sqldialog.ui \
    editdialog.ui \
    relationshipdialog.ui \
    entitydialog.ui \
    hierarchydialog.ui

RESOURCES += \
    images/images.qrc
