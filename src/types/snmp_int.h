#ifndef SNMP_INT_H
#define SNMP_INT_H

#include "snmp_type.h"
#include "types.h"
#include "lenghtcoder.h"

#include <iostream>
#include <stdio.h>

class SNMP_LIBRARY_EXPORT SNMP_int : public SNMP_type
{
    static constexpr int ARRAY_SIZE = 10;

    bool isValid_;
    int value_;

//    uint8_t *encode(int value, uint8_t *buffer, int bufferSize);
//    void decode(uint8_t *pData, int dataLen);

public:
    SNMP_int();
    SNMP_int(int value);

    virtual ~SNMP_int() {}

    bool isValid() const;
    int getValue(bool *isValid = NULL) const;
    virtual void setValue(int value);

    // SNMP_type interface
public:
    ///
    /// \brief copyToSNMP ... nakopiruje cislo 'value_' na danou pozici v packetu
    /// \param pData ... ukazatel do snmp packetu na misto, kam chci zapsat data
    /// \param dataLen ... zbyvajici velikost packetu
    /// \return ... vraci pocet zapsanych bajtu
    ///
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;

    ///
    /// \brief readFromSNMP ... cte z predaneho mista snmp packetu cele cislo
    /// \param pData ... ukazatel do prijateho packetu na hodnotu int
    /// \param dataLen ... delka, kolik bajtu urcuje hodnotu
    /// \return
    ///
    virtual bool readFromSNMP(snmp::itemTLV &item) override;

    ///
    /// \brief getItemSize ... vraci velikost TLV, ktere je treba zapsat do snmp packetu
    /// \return
    ///
    virtual int getItemSize() override;

    ///
    /// \brief getType ... vraci typ polozky
    /// \return
    ///
    virtual uint8_t getType() override;

    ///
    /// \brief toString ... vraci hodnotu jako string
    /// \return
    ///
    virtual std::string toString() override;

};

#endif // SNMP_INT_H
