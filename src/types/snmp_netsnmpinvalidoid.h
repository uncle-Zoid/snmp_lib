#ifndef SNMP_NETSNMPINVALIDOID_H
#define SNMP_NETSNMPINVALIDOID_H

#include <string>

#include "snmp_type.h"

class SNMP_LIBRARY_EXPORT SNMP_netsnmpInvalidOid : public SNMP_type
{
public:
    SNMP_netsnmpInvalidOid();
    bool isValid() const;

    // SNMP_type interface
public:
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;
    virtual uint8_t getType() override;
    virtual int getItemSize() override;
    virtual std::string toString() override;
};


#endif // SNMP_NETSNMPINVALIDOID_H
