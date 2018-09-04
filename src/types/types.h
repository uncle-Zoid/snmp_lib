#ifndef TYPES_H
#define TYPES_H

// primitive

#define TAG_BOOLEAN      0x01
#define TAG_INT          0x02
#define TAG_OBJ          0x06
#define TAG_OCTET_STRING 0x04
#define TAG_NULL         0x05
#define TAG_IP           0x40
#define TAG_UNSIGNED     0x42
#define TAG_NETSNMP_INVALID_OID 0x80 // polozka posilana od net-snmp, kdyz zadam o neexistujici prvek

#define TAG_SEQ          0x30

#define PDU_GET       0xA0
#define PDU_GET_NEXT  0xA1
#define PDU_RESPONSE  0xA2
#define PDU_SET       0xA3
#define PDU_TRAP      0xA4

#define NON_VALID_TAG  -1


//
#define TAG_SEQ 0x30

#include <memory>
#include <cstdint>
#include <string>
#include "snmp_type.h"
#include "snmp_object.h"

namespace snmp
{

    struct SNMP_LIBRARY_EXPORT itemTLV
    {
        uint8_t tag;
        int length;
        uint8_t *pValue;
    };

    struct SNMP_LIBRARY_EXPORT itemVarBinding
    {
        std::shared_ptr<SNMP_type> value;
//        std::string oid;
        std::shared_ptr<SNMP_object> oid;
    };
}

#endif // TYPES_H
