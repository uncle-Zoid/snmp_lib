#include <vector>
#include <cstring>

#include "snmp_ipaddress4.h"
#include "lenghtcoder.h"
#include "snmp_octetstring.h"
#include "types.h"

using namespace std;

SNMP_ipAddress4::SNMP_ipAddress4()
    :isValid_(false)
{ }

void SNMP_ipAddress4::setValue(uint8_t *value)
{
    memcpy(ip_, value, IP_BYTES_LEN);
}




int SNMP_ipAddress4::copyToSNMP(uint8_t *pData, int dataLen)
{
    int toWrite = getItemSize();

    if(toWrite > dataLen) // pokud neni v packetu jiz misto
        return -1;

    *pData ++ = getType();
    *pData ++ = toWrite;
    memcpy(pData, ip_, IP_BYTES_LEN);

    return toWrite;
}

bool SNMP_ipAddress4::readFromSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_octetString::readFromSNMP(uint8_t *pData, int dataLen) not implemented!!!";
}

bool SNMP_ipAddress4::readFromSNMP(snmp::itemTLV &item)
{
    isValid_ = false;
    if (item.tag != getType() || item.length != IP_BYTES_LEN)
        return false;

    memcpy(ip_, item.pValue, IP_BYTES_LEN);
    isValid_ = true;
    return true;
}

uint8_t SNMP_ipAddress4::getType()
{
    return TAG_IP;
}

int SNMP_ipAddress4::getItemSize()
{
    return 6;
}

string SNMP_ipAddress4::toString()
{
    char buff[100];
    snprintf(buff, sizeof(buff), "%d.%d.%d.%d", ip_[0], ip_[1], ip_[2], ip_[3]);

    return buff;
}
