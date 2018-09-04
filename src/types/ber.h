#ifndef BER_H
#define BER_H

#include <stdint.h>

class BER
{

    BER();
public:
    static uint8_t *encode(int32_t value, uint8_t *buffer, int bufferSize)
    {
        uint8_t *pActualByte = buffer + bufferSize - 1;
        bool isNegative = value < 0;

        *pActualByte = 0;
        // pokud je cislo kladne a msb == '1', je potreba vlozit na MSB 0x00 -> jinak chapano jako zaporne cislo
        do
        {
            *pActualByte -- = value & 0xFF;
            value >>= 8;
            if(value == (-1)) break;
        }while((value != 0) /*&& (pActualByte != bufferForEncodedValue_ - 1)*/);

        if((*(pActualByte+1)& 0x80)>>7 != isNegative)
        {
            *pActualByte-- = isNegative?0xFF:0x00;
        }

        return ++pActualByte;
    }


    static uint8_t *encode(uint32_t value, uint8_t *buffer, int bufferSize)
    {
        uint8_t *pActualByte = buffer + bufferSize - 1;
        *pActualByte = 0;
        // pokud je cislo kladne a msb == '1', je potreba vlozit na MSB 0x00 -> jinak chapano jako zaporne cislo
        do
        {
            *pActualByte -- = value & 0xFF;
            value >>= 8;
        }while((value != 0) /*&& (pActualByte != bufferForEncodedValue_ - 1)*/);

        if((*(pActualByte+1)& 0x80)>>7)
        {
            *pActualByte-- = 0x00;
        }

        return ++pActualByte;
    }


    static int64_t decode(uint8_t *pData, int dataLen)
    {
        uint8_t *pNu = pData;
        int64_t value = (*pNu & 0x80) ? -1 : 0;

        const uint8_t *pDataEnd = (pData + dataLen);
        while(pNu != pDataEnd)
        {
            value <<= 8;
            value |= *pNu++ & 0xFF;
        }
        return value;
    }


};

#endif // BER_H
