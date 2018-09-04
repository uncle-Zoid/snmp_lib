#include "snmp_null.h"
#include "snmpdata.h"

SNMP_null::SNMP_null()
{   
}

bool SNMP_null::isValid() const
{
    return true;
}

int SNMP_null::copyToSNMP(uint8_t *pData, int dataLen)
{
    if(dataLen < getItemSize())
        return -1;

    *pData = getType();     // T
    *(pData + 1) = 0x00;    // L

    return getItemSize();
}

bool SNMP_null::readFromSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_null::readFromSNMP(uint8_t *pData, int dataLen) not implemented!!!";
}

bool SNMP_null::readFromSNMP(snmp::itemTLV &item)
{
    (void)item;
    return true;
}

uint8_t SNMP_null::getType()
{
    return TAG_NULL;
}

int SNMP_null::getItemSize()
{
    return 2; // T=null L=0
}

std::string SNMP_null::toString()
{
    return "null";
}
