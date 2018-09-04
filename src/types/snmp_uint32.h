#ifndef SNMP_UINT32_H
#define SNMP_UINT32_H

#include "snmp_type.h"
#include "types.h"

class SNMP_LIBRARY_EXPORT SNMP_uint32 : public SNMP_type/*: public SNMP_int*/
{
    static constexpr int ARRAY_SIZE = 10;
    uint32_t value_;
    bool isValid_ = false;

public:
    SNMP_uint32();
    SNMP_uint32(uint32_t value);


    bool isValid() const;
    uint32_t getValue(bool *isValid = NULL) const;
    void setValue(uint32_t value);


    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;

    ///
    /// \brief SNMP_int::getItemSize ... vraci celkovy pocet bajtu potrebnych pro zapsani SNMP_int do packetu.
    ///                                  Struktura je  T ... <1B>, L ... <1B>, V ... <1-5B>
    /// \return
    ///
    virtual int getItemSize() override;


    virtual uint8_t getType() override
    {
        return TAG_UNSIGNED;
    }

    virtual std::string toString() override
    {
        return std::to_string(value_);
    }
};

#endif // SNMP_UINT32_H
