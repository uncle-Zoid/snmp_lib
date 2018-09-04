#include "snmp_netsnmpinvalidoid.h"
#include "snmpdata.h"

SNMP_netsnmpInvalidOid::SNMP_netsnmpInvalidOid()
{
}

bool SNMP_netsnmpInvalidOid::isValid() const
{
    return true;
}

int SNMP_netsnmpInvalidOid::copyToSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_netsnmpInvalidOid::copyToSNMP ... NENI URCENO PRO MUJ ZAPIS, MUZE SE OBJEVIT JEN PRI CTENI. TOHLE NEZAPISUJ!!!";
}

bool SNMP_netsnmpInvalidOid::readFromSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_netsnmpInvalidOid::readFromSNMP(uint8_t *pData, int dataLen) not implemented!!!";
}

bool SNMP_netsnmpInvalidOid::readFromSNMP(snmp::itemTLV &item)
{
    (void)item;
    return true;
}

uint8_t SNMP_netsnmpInvalidOid::getType()
{
    return TAG_NETSNMP_INVALID_OID;
}

int SNMP_netsnmpInvalidOid::getItemSize()
{
    return 2; // T=null L=0
}

std::string SNMP_netsnmpInvalidOid::toString()
{
    return "netsnmpInvalidOid";
}
