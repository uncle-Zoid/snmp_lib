#include "snmp_boolean.h"
#include "types.h"

#include <string>

SNMP_boolean::SNMP_boolean()
    : value_(false)
    , isValid_(false)
    { }

SNMP_boolean::SNMP_boolean(bool value)
    : value_(value)
    , isValid_(true)
    { }

int SNMP_boolean::copyToSNMP(uint8_t *pData, int dataLen)
{
    if(dataLen < getItemSize())
        return -1;

    *pData = getType();
    *(pData + 1) = 0x01;
    *(pData + 2) = value_ ? 0xFF : 0x00;

    return getItemSize();
}

bool SNMP_boolean::readFromSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_boolean::readFromSNMP(uint8_t *pData, int dataLen) not implemented!!!";
}

bool SNMP_boolean::readFromSNMP(snmp::itemTLV &item)
{
    isValid_ = (item.tag == TAG_BOOLEAN);
    value_ = *item.pValue > 0x00;
    return isValid_;
}

uint8_t SNMP_boolean::getType()
{
    return TAG_BOOLEAN;
}

int SNMP_boolean::getItemSize()
{
    return 3;
}

std::string SNMP_boolean::toString()
{
    return value_ ? "true":"false";
}
