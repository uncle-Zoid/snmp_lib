include( common.pri )

QT += core network
QT -= gui

CONFIG += c++11

TARGET = snmp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = lib
#TEMPLATE = app

DEFINES += SNMP_LIBRARY

SOURCES += \
    types/snmp_int.cpp \
    types/pdu.cpp \
    types/snmp_object.cpp \
    types/lenghtcoder.cpp \
    types/varbinding.cpp \
    types/snmp_octetstring.cpp \
    types/snmpdata.cpp \
    types/snmp_null.cpp \
    types/snmp_boolean.cpp \
    types/snmp_uint32.cpp \
    types/ber.cpp \
    types/snmp_ipaddress4.cpp \
    types/snmp_netsnmpinvalidoid.cpp \
    snmpparser.cpp \

#    main.cpp \


HEADERS += \
    types/snmp_int.h \
    types/snmp_type.h \
    types/pdu.h \
    types/snmp_object.h \
    types/types.h \
    types/lenghtcoder.h \
    types/varbinding.h \
    types/snmp_octetstring.h \
    types/snmpdata.h \
    types/snmp_null.h \
    types/snmp_boolean.h \
    types/snmp_uint32.h \
    types/ber.h \
    types/snmp_ipaddress4.h \
    types/snmp_netsnmpinvalidoid.h \
    snmpparser.h \
    snmplib.h



#win32:
#{
#{
#CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
#CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

win32:dlltarget.files = $$files(*.dll)
win32:dlltarget.files += $$files(libsnmp*)
#dlltarget.files += $$files(*.a,true)
#dll/*.dll
#dlltarget.path = ../mssr_diagnostic/mssr_cms/libs/lib/win
win32:dlltarget.path = P:/GIT/mssr/mssr_diagnostic/mssr_cms/libs/lib/win
INSTALLS += dlltarget
#}

#libraryFiles.path = $$[QT_INSTALL_LIBS]
#CONFIG(debug, debug|release):libraryFiles.files = $$OUT_PWD/debug/*.a $$OUT_PWD/debug/*.prl
#CONFIG(release, debug|release):libraryFiles.files = $$OUT_PWD/release/*.a $$OUT_PWD/release/*.prl
#INSTALLS += libraryFiles



