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
    snmpparser.cpp \
    types/varbinding.cpp \
    types/snmp_octetstring.cpp \
    types/snmpdata.cpp \
    types/snmp_null.cpp \
    types/snmp_boolean.cpp \
    types/snmp_uint32.cpp \
    types/ber.cpp \
    snmp_manager_request.cpp \
    types/snmp_ipaddress4.cpp \
    types/snmp_netsnmpinvalidoid.cpp \
#    main.cpp \


HEADERS += \
    types/snmp_int.h \
    types/snmp_type.h \
    types/pdu.h \
    types/snmp_object.h \
    types/types.h \
    types/lenghtcoder.h \
    snmpparser.h \
    types/varbinding.h \
    types/snmp_octetstring.h \
    types/snmpdata.h \
    types/snmp_null.h \
    types/snmp_boolean.h \
    types/snmp_uint32.h \
    types/ber.h \
    snmp_manager_request.h \
    types/snmp_ipaddress4.h \
    types/snmp_netsnmpinvalidoid.h \
    snmplib.h
