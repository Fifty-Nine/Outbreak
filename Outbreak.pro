TEMPLATE = app

MOC_DIR = Mocs/
OBJECTS_DIR = Objs/
UICS_DIR = Uics/

QT += opengl

win32 {
    INCLUDEPATH += "C:\\Users\\princet\\Downloads\\boost_1_49_0\\boost_1_49_0"
    INCLUDEPATH += .
    QMAKE_CXXFLAGS += /openmp
}
unix {
    INCLUDEPATH += /home/princet/Downloads/boost_1_46_0
    QMAKE_CXXFLAGS += -std=c++0x -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

HEADERS += \

SOURCES += \
    Main.cpp \

include(Entities/Entities.pri)
include(Gui/Gui.pri)
include(Map/Map.pri)
