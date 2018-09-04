#include <cstring>

#include "snmp_int.h"
#include "ber.h"

using namespace std;

SNMP_int::SNMP_int()
{
    isValid_ = false;
}

SNMP_int::SNMP_int(int value)
{
    setValue(value);
}


bool SNMP_int::isValid() const
{
    return isValid_;
}

int SNMP_int::getValue(bool *isValid) const
{
    if(isValid != NULL)
        *isValid = isValid_;

    return value_;
}

void SNMP_int::setValue(int value)
{
    value_ = value;
    isValid_ = true;
}


int SNMP_int::copyToSNMP(uint8_t *pData, int dataLen)
{
    int toWrite = getItemSize();                        //  celkovy pocet bajtu pro polozku TLV
    uint8_t valueLenght = getItemSize() - 2;            // T a L zaberou VZDY 2 bajty, zbytek je pro V <1 ; 4> bajty => L lze ukladat jako singleBajt

    if(toWrite > dataLen) // pokud neni v packetu jiz misto
        throw " SNMP_int::copyToSNMP ... malo mista v packetu";

    // typ int32_t zabere < 1 ; 4 > bajty       po zakodobani bude cislo ulozeno v hornich bajtech 'buffer'. Zacatek dat je v 'pNo'
    uint8_t buffer[ARRAY_SIZE];
    uint8_t *pNo = BER::encode(value_, buffer, ARRAY_SIZE);

    *pData ++  = getType();
    *pData ++  = valueLenght;
    memcpy(pData, pNo, (buffer + ARRAY_SIZE) - pNo);

    return toWrite;
}

bool SNMP_int::readFromSNMP(snmp::itemTLV &item)
{
    isValid_ = false;
    if (item.tag != getType() || item.length > 4) // int32_t nemuze byt zakodovan ve vice jak 4 bajtech
        return isValid_;

    value_ = (int)BER::decode(item.pValue, item.length);
    isValid_ = true;

    return isValid_;
}

///
/// \brief SNMP_int::getItemSize ... vraci celkovy pocet bajtu potrebnych pro zapsani SNMP_int do packetu.
///                                  Struktura je  T ... <1B>, L ... <1B>, V ... <1-4B>
/// \return
///
int SNMP_int::getItemSize()
{
    // predpokladam, ze na zakodovani delky int staci jeden bajt a typ druhy bajt ... T=1, L=1
    if(value_ < 0x80 && value_ >= -0x80)         return 3;  // pro hodnoty < -128 ; 127 >                       staci 1 bajt  -> + hlavicka => 3 bajty celkove
    if(value_ < 0x8000 && value_ >= -0x8000)     return 4;  // pro hodnoty < -32 768 ; 32 767 >                 staci 2 bajty -> + hlavicka => 4 bajty celkove
    if(value_ < 0x800000 && value_ >= -0x800000) return 5;  // pro hodnoty < -8 388 608 ;  8 388 607 >          staci 3 bajty -> + hlavicka => 5 bajty celkove
    return 6;                                               // pro hodnoty < -2 147 483 648 ; 2 147 483 647 >   staci 4 bajty -> + hlavicka => 6 bajty celkove
}

uint8_t SNMP_int::getType()
{
    return TAG_INT;
}

string SNMP_int::toString()
{
    return std::to_string(value_);
}

