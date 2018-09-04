#ifndef SNMPLIB_H
#define SNMPLIB_H

#include <QtCore/QtGlobal>

#if defined(SNMP_LIBRARY)
#  define SNMP_LIBRARY_EXPORT Q_DECL_EXPORT
#else
#  define SNMP_LIBRARY_EXPORT Q_DECL_IMPORT
#endif

#endif // SNMPLIB_H
