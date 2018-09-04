#ifndef SNMP_IPADDRESS_H
#define SNMP_IPADDRESS_H

#include "snmp_type.h"

class SNMP_LIBRARY_EXPORT SNMP_ipAddress4 : public SNMP_type
{
    static constexpr int IP_BYTES_LEN = 4;

    uint8_t ip_[4];
    bool isValid_;

public:
     SNMP_ipAddress4();
     virtual ~SNMP_ipAddress4() {}

     void setValue(std::string value)
     {
         (void)value;
         //value_ = "0000";
     }
     void setValue(uint8_t *value);

     const uint8_t *getValue()
     {
         return ip_;
     }

     bool isValid()const
     {
         return isValid_;
     }

    // SNMP_type interface
public:
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) override;
    virtual uint8_t getType() override;
    virtual int getItemSize() override;
    virtual std::string toString() override;
};

#endif // SNMP_IPADDRESS_H
