######################################################################
# Automatically generated by qmake (2.01a) Mo Mai 21 18:56:28 2007
######################################################################

CONFIG += debug_and_release
TEMPLATE = app
TARGET = FreeSSM
DESTDIR = ./
DEPENDPATH += . src ui
INCLUDEPATH += . src src/tinyxml


# Input
HEADERS += src/FreeSSM.h \
           src/Languages.h \
           src/EngineDialog.h \
           src/TransmissionDialog.h \
           src/Preferences.h \
           src/About.h \
           src/FSSMdialogs.h \
           src/ActuatorTestDlg.h \
           src/AbstractDiagInterface.h \
           src/SerialPassThroughDiagInterface.h \
           src/J2534DiagInterface.h \
           src/J2534.h \
           src/SSMP1communication.h \
           src/SSMP1communication_procedures.h \
           src/SSMP1base.h \
           src/SSMP2communication.h \
           src/SSMP2communication_core.h \
           src/SSMprotocol.h \
           src/SSMprotocol1.h \
           src/SSMprotocol2.h \
           src/AddMBsSWsDlg.h \
           src/ControlUnitDialog.h \
           src/CUinfo_Engine.h \
           src/CUinfo_Transmission.h \
           src/CUcontent_DCs_abstract.h \
           src/CUcontent_DCs_engine.h \
           src/CUcontent_DCs_transmission.h \
           src/CUcontent_MBsSWs.h \
           src/CUcontent_MBsSWs_tableView.h \
           src/CUcontent_Adjustments.h \
           src/CUcontent_sysTests.h \
           src/SSM1definitionsInterface.h \
           src/SSMprotocol2_ID.h \
           src/SSMprotocol2_def_en.h \
           src/SSMprotocol2_def_de.h \
           src/ClearMemoryDlg.h \
           src/libFSSM.h \
           src/tinyxml/tinyxml.h \
           src/tinyxml/tinystr.h

SOURCES += src/main.cpp \
           src/FreeSSM.cpp \
           src/EngineDialog.cpp \
           src/TransmissionDialog.cpp \
           src/Preferences.cpp \
           src/About.cpp \
           src/FSSMdialogs.cpp \
           src/ActuatorTestDlg.cpp \
           src/AbstractDiagInterface.cpp \
           src/SerialPassThroughDiagInterface.cpp \
           src/J2534DiagInterface.cpp \
           src/SSMP1communication.cpp \
           src/SSMP1communication_procedures.cpp \
           src/SSMP1base.cpp \
           src/SSMP2communication.cpp \
           src/SSMP2communication_core.cpp \
           src/SSMprotocol.cpp \
           src/SSMprotocol1.cpp \
           src/SSMprotocol2.cpp \
           src/AddMBsSWsDlg.cpp \
           src/ControlUnitDialog.cpp \
           src/CUinfo_Engine.cpp \
           src/CUinfo_Transmission.cpp \
           src/CUcontent_DCs_abstract.cpp \
           src/CUcontent_DCs_engine.cpp \
           src/CUcontent_DCs_transmission.cpp \
           src/CUcontent_MBsSWs.cpp \
           src/CUcontent_MBsSWs_tableView.cpp \
           src/CUcontent_Adjustments.cpp \
           src/CUcontent_sysTests.cpp \
           src/SSM1definitionsInterface.cpp \
           src/SSMprotocol2_ID.cpp \
           src/SSMprotocol2_def_en.cpp \
           src/SSMprotocol2_def_de.cpp \
           src/ClearMemoryDlg.cpp \
           src/libFSSM.cpp \
           src/tinyxml/tinyxml.cpp \
           src/tinyxml/tinystr.cpp \
           src/tinyxml/tinyxmlerror.cpp \
           src/tinyxml/tinyxmlparser.cpp

FORMS +=   ui/FreeSSM.ui \
           ui/Preferences.ui \
           ui/About.ui \
           ui/ControlUnitDialog.ui \
           ui/CUinfo_Engine.ui \
           ui/CUinfo_Transmission.ui \
           ui/CUcontent_DCs_engine.ui \
           ui/CUcontent_DCs_transmission.ui \
           ui/CUcontent_MBsSWs.ui \
           ui/CUcontent_MBsSWs_tableView.ui \
           ui/CUcontent_Adjustments.ui \
           ui/CUcontent_sysTests.ui \
           ui/AddMBsSWsDlg.ui \
           ui/ActuatorTestDlg.ui

RESOURCES += resources/FreeSSM.qrc

TRANSLATIONS = FreeSSM_en.ts \
               FreeSSM_de.ts

translation.commands = lrelease FreeSSM.pro & qmake
QMAKE_EXTRA_TARGETS += translation

DEFINES += TIXML_USE_STL
# Add pre-processor-define if we compile as debug:
CONFIG(debug, debug|release): DEFINES += __FSSM_DEBUG__ __SERIALCOM_DEBUG__ __J2534_API_DEBUG__

# disable gcse-optimization (regressions with gcc-versions >= 4.2)
QMAKE_CXXFLAGS += -fno-gcse          # disable gcse-optimization (regressions with gcc-versions >= 4.2)

# Installation
unix:INSTALLDIR = $$system(echo ~)/FreeSSM
win32:INSTALLDIR = $$system(echo %programfiles%)/FreeSSM
target.path = $$INSTALLDIR
filestarget.path = $$INSTALLDIR
filestarget.files = background.png LiberationSans*.ttf *.qm
unix:filestarget.files += resources/icons/freessm/48x48/FreeSSM.png
doctarget.path = $$INSTALLDIR/doc
doctarget.files = doc/*
INSTALLS += target doctarget filestarget




# OS-specific options
unix {
       DEPENDPATH += src/linux
       INCLUDEPATH += src/linux
       HEADERS += src/linux/serialCOM.h \
                  src/linux/TimeM.h \
                  src/linux/J2534_API.h
       SOURCES += src/linux/serialCOM.cpp \
                  src/linux/TimeM.cpp \
                  src/linux/J2534_API.cpp
       LIBS += -ldl -lrt
}

win32 {
       CONFIG(debug, debug|release): CONFIG += console
       DEPENDPATH += src/windows
       INCLUDEPATH += src/windows
       HEADERS += src/windows/serialCOM.h \
                  src/windows/TimeM.h \
                  src/windows/J2534_API.h
       SOURCES += src/windows/serialCOM.cpp \
                  src/windows/TimeM.cpp \
                  src/windows/J2534_API.cpp
       RC_FILE = resources/FreeSSM_WinAppIcon.rc
}


