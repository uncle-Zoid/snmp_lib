#ifndef SNMP_BOOLEAN_H
#define SNMP_BOOLEAN_H

#include "snmp_type.h"

class SNMP_LIBRARY_EXPORT SNMP_boolean : public SNMP_type
{
    bool value_;
    bool isValid_;

public:
    SNMP_boolean();
    SNMP_boolean(bool value);

    bool isValid()const
    {
        return isValid_;
    }

    // SNMP_type interface
public:
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;
    virtual uint8_t getType() override;
    virtual int getItemSize() override;
    virtual std::string toString() override;
};

#endif // SNMP_BOOLEAN_H
