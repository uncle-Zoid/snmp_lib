#include <cstring>

#include "snmp_uint32.h"
#include "ber.h"

SNMP_uint32::SNMP_uint32()
    : isValid_(false)
{}

SNMP_uint32::SNMP_uint32(uint32_t value)
{
    setValue(value);
}

bool SNMP_uint32::isValid() const
{
    return isValid_;
}

uint32_t SNMP_uint32::getValue(bool *isValid) const
{
    if(isValid != NULL)
        *isValid = isValid_;

    return value_;
}

void SNMP_uint32::setValue(uint32_t value)
{
    value_ = value;
    isValid_ = true;
}

int SNMP_uint32::copyToSNMP(uint8_t *pData, int dataLen)
{
    int toWrite = getItemSize();                        //  celkovy pocet bajtu pro polozku TLV
    uint8_t valueLenght = getItemSize() - 2;            // T a L zaberou VZDY 2 bajty, zbytek je pro V <1 ; 4> bajty => L lze ukladat jako singleBajt

    if(toWrite > dataLen) // pokud neni v packetu jiz misto
        return -1;

    // typ int32_t zabere < 1 ; 4 > bajty       po zakodobani bude cislo ulozeno v hornich bajtech 'buffer'. Zacatek dat je v 'pNo'
    uint8_t buffer[ARRAY_SIZE];
    uint8_t *pNo = BER::encode(value_, buffer, ARRAY_SIZE);

    *pData ++  = getType();
    *pData ++  = valueLenght;
    memcpy(pData, pNo, (buffer + ARRAY_SIZE) - pNo);

    return toWrite;
}

bool SNMP_uint32::readFromSNMP(uint8_t *pData, int dataLen)
{
    //    (void)pData; (void)dataLen;
    //    throw "bool SNMP_int::readFromSNMP(uint8_t *pData, int dataLen) ... not implemented";
    if ((*pData ++) != getType())
        return false;

    uint8_t *pItem = pData;
    int len = LenghtCoder::decodeItemsLenght(pItem, (const uint8_t *)(pItem+dataLen));

    isValid_ = (len <= 5);   // uint32_t nemuze byt zakodovan ve vice jak 5 bajtech
    if(isValid_)
        value_ = (int)BER::decode(pItem, len);

    return isValid_;
}

bool SNMP_uint32::readFromSNMP(snmp::itemTLV &item)
{
    isValid_ = false;
    if (item.tag != getType() || item.length > 5) // uint32_t nemuze byt zakodovan ve vice jak 5 bajtech
        return isValid_;

    value_ = (int)BER::decode(item.pValue, item.length);
    isValid_ = true;

    return isValid_;
}

int SNMP_uint32::getItemSize()
{
    // predpokladam, ze na zakodovani delky int staci jeden bajt a typ druhy bajt ... T=1, L=1
    if(value_ < 0x80)         return 3;  // pro hodnoty < 0 ; 127 >                       staci 1 bajt  -> + hlavicka => 3 bajty celkove
    if(value_ < 0x8000)       return 4;  // pro hodnoty < 128 ; 32 767 >                  staci 2 bajty -> + hlavicka => 4 bajty celkove
    if(value_ < 0x800000)     return 5;  // pro hodnoty < 32 768 ; 8 388 607 >            staci 3 bajty -> + hlavicka => 5 bajty celkove
    if(value_ < 0x80000000)   return 6;  // pro hodnoty < 8 388 608 ; 2 147 483 647 >     staci 4 bajty -> + hlavicka => 6 bajty celkove
    return 7;  // pro hodnoty < 2 147 483 647 ; 4 294 967 295 > staci 5 bajtu -> + hlavicka => 7 bajtu celkove
}
