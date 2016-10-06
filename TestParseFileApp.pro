TEMPLATE = app

QT += qml quick
CONFIG += c++11

HEADERS += \
    datacodes.h \
    uiprocess.h \
    ParseFile/Logging/logging.h \
    ParseFile/QMLParse/qmlparse.h \
    ParseFile/QMLParse/qmlparsefunction.h \
    ParseFile/parsedirectory.h \
    ParseFile/parsefiles.h \
    ParseFile/parsefunctions.h \
    ParseFile/QMLParse/qmldatacodes.h \
    ParseFile/QMLParse/qmluiprocess.h

SOURCES += main.cpp \
    datacodes.cpp \
    uiprocess.cpp \
    ParseFile/Logging/logging.cpp \
    ParseFile/QMLParse/qmlparse.cpp \
    ParseFile/QMLParse/qmlparsefunction.cpp \
    ParseFile/parsedirectory.cpp \
    ParseFile/parsefiles.cpp \
    ParseFile/parsefunctions.cpp \
    ParseFile/QMLParse/qmldatacodes.cpp \
    ParseFile/QMLParse/qmluiprocess.cpp

INCLUDEPATH += $$PWD/../../../../../../Users/Dude/Documents/Development/Qt_development/TestParseFileApp/ParseFile/
INCLUDEPATH += $$PWD/../../../../../../Boost_1.61/boost_1_61_0/

LIBS += -L../../../../../../../Boost_1.61/boost_1_61_0/lib64-msvc-12.0/

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    ParseFile/Logging/Logging_implementation.txt \
    ParseFile/QMLParse/qmlpstr notes.txt \
    ParseFile/ReadMe.txt
