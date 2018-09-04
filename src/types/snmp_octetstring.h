#ifndef SNMP_OCTETSTRING_H
#define SNMP_OCTETSTRING_H

#include "snmp_type.h"
#include <string>

class SNMP_LIBRARY_EXPORT SNMP_octetString : public SNMP_type
{
    std::string value_;
    bool isValid_;

public:
    SNMP_octetString();
    SNMP_octetString(std::string value);
    std::string getValue() const;
    void setValue(std::string value);

    bool isValid() const
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

#endif // SNMP_OCTETSTRING_H
