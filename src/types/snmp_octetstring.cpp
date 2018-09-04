#include <vector>
#include <cstring>

#include "lenghtcoder.h"
#include "snmp_octetstring.h"
#include  "types.h"

using namespace std;

SNMP_octetString::SNMP_octetString()
    :isValid_(false)
    { }

SNMP_octetString::SNMP_octetString(string value)
{
    setValue(value);
}



string SNMP_octetString::getValue() const
{
    return value_;
}

void SNMP_octetString::setValue(string value)
{
    value_ = value;
    isValid_ = true;
}

int SNMP_octetString::copyToSNMP(uint8_t *pData, int dataLen)
{
    uint8_t bufferForLenght[5];
    int lenoflen = LenghtCoder::encodeLenght(bufferForLenght, sizeof(bufferForLenght), value_.size()); //getLenghtOfLenght(value_.size());
    int toWrite = 1 + lenoflen + value_.size();

    if(toWrite > dataLen) // pokud neni v packetu jiz misto
        return -1;

    *pData = getType();
    memcpy(pData + 1, bufferForLenght, lenoflen);
    memcpy(pData + lenoflen + 1, value_.data(), value_.size());

    return toWrite;
}

bool SNMP_octetString::readFromSNMP(uint8_t *pData, int dataLen)
{
    (void)pData; (void)dataLen;
    throw "SNMP_octetString::readFromSNMP(uint8_t *pData, int dataLen) not implemented!!!";
}

bool SNMP_octetString::readFromSNMP(snmp::itemTLV &item)
{
    if (item.tag != getType())
        return false;

    value_.assign((const char*)item.pValue, item.length);
    isValid_ = true;
    return true;
}

uint8_t SNMP_octetString::getType()
{
    return TAG_OCTET_STRING;
}

int SNMP_octetString::getItemSize()
{
    return 1 + LenghtCoder::getLenghtOfLenght(value_.size()) + value_.size();
}

string SNMP_octetString::toString()
{
    return getValue();
}
