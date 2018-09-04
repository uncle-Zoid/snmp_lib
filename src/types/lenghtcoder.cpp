#include "lenghtcoder.h"

LenghtCoder::LenghtCoder()
{

}

int LenghtCoder::encodeLenght(uint8_t * const pBuffer, int bufferLen, int value)
{    
    if(pBuffer == NULL)
        return -1;

    int totalBytes = 1;
    if(value > 127) // ulozit delku vicebajtove
    {
        // prvni bajt obsahuje 0b1xxx xxxx, kde x koduji pocet nasledujicich bajtu, ktere obsahuji delku v big-endian
        int pomLenght = value;
        while(pomLenght >>= 8)
            ++ totalBytes;

        ++ totalBytes;// +1 ... prvni bajt pro extra buřty
        if(totalBytes > bufferLen) // nevejdou se
            return -1;

        *pBuffer = 0x80 | (totalBytes - 1); // prvni bajt obsahuje 0x80 a pocet nasledujicich bajtu se zakodovanou hodnotou
        int index = totalBytes - 1;
        do
        {
            *(pBuffer + index) = value & 0xFF;
            -- index;
        }while(value >>= 8);
    }
    else
    {
        *pBuffer = uint8_t(value);
    }
    return totalBytes;
}

int LenghtCoder::decodeItemsLenght(uint8_t *&data, const uint8_t *dataEnd)
{
    int remainingData = distance((const uint8_t *)data, dataEnd);

    // delka polozky muze byt v jednom, nebo vice bajtech
    return ((*data) & 0x80)
            ? decodeItemsLenght_multibytes(data, remainingData)
            : decodeItemsLenght_singlebytes(data, remainingData);
}


int LenghtCoder::decodeItemsLenght_multibytes(uint8_t *&data, int remainingData)
{
    if(remainingData <= 0)
    {
        return -1; // neni dost dat
    }

    if(((*data) & 0x7F) == 0) // nespec. delka polozky, polozka konci typen EOC
    {
        ++ data;
        return 0;
    }

    // delka je vicebajtova ... *(data) = n=pocet bajtu delky, *(data+1)...*(data+n)=delka polozky v bajtech
    int len = (*data) & 0x7F;
    if (len > (int)sizeof(int) || len > remainingData) // kdyz bude delka ve vic jak 4 bajtech,
    {
        cout << "Velikost datove polozky je ulozena ve vice jak 4 bajtech. s tim si zatim neporadim";
        //        data += len;
        return -1; // chyba cteni, polozka ma moc velkou delku
    }

    int value = 0;
    for (int i = 1; i <= len; ++i)
    {
        value |= *(data + i) << 8*(len - i); // delka ulozena v big-endian
    }
    data += len + 1;// ukazuje na dalsi polozku, nebo hodnotu polozky
    return value;
}

int LenghtCoder::decodeItemsLenght_singlebytes(uint8_t *&pData, int remainingData)
{
    if(remainingData <= 0)
    {
        return -1; // neni dost dat
    }

    return *pData++;
}


///
/// \brief LenghtCoder::getLenghtOfLenght ... vraci pocet bajtu, ktery je potreba pro zakodovani cisla jako delky v snmp
/// \param lenght ... delka polozky
/// \return ... pocet bajtu potrebny k zakodovani delky
///
int LenghtCoder::getLenghtOfLenght(int lenght)
{
    int totalBytes = 1;
    if(lenght >= 0x80) // pokud je delka polozky > 127, potreba vice bajtu + 1bajt extra
    {
        do
        {
            ++ totalBytes;
        }
        while(lenght >>= 8);
    }
    return totalBytes;
}
