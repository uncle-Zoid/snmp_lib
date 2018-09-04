#ifndef LENGHTCODER_H
#define LENGHTCODER_H

#include <cstdint>
#include <iostream>

using namespace std;

class LenghtCoder
{
private:
    LenghtCoder();

public: 

    ///
    /// \brief lenght2Array ... zapise predanou delku do predaneho bufferu podle BER
    /// \param pBuffer
    /// \param bufferLen
    /// \param lenght ... delka k zakodovani
    /// \return ... pocet platnych bajtu v bufferu
    ///
    static int encodeLenght(uint8_t *const pBuffer, int bufferLen, int value);



    ///
    /// \brief decodeItemsLenght ... precte delku polozky T'L'V a zaroven presune ukazatel 'pData' na zacatek bloku hodnoty (V)
    /// \param data ... ukazatel na zacatek bloku delka (L)
    /// \param dataEnd  ... ukazatel na konec packetu
    /// \return
    ///
    static int decodeItemsLenght(uint8_t *&data, const uint8_t *dataEnd);


    static int getLenghtOfLenght(int lenght);


private:
    static int decodeItemsLenght_multibytes(uint8_t *&data, int remainingData);
    static int decodeItemsLenght_singlebytes(uint8_t *&pData, int remainingData);



};

#endif // LENGHTCODER_H
