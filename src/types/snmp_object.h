#ifndef SNMP_OBJECT_H
#define SNMP_OBJECT_H

#include "snmp_type.h"
//#include "types/types.h"
#include "lenghtcoder.h"

#include <string>
#include <vector>

namespace snmp {
    struct itemTLV;
}

class SNMP_LIBRARY_EXPORT SNMP_object : public SNMP_type
{
    std::vector<uint8_t> encodedOid_;
    std::vector<uint8_t> toSnmp_;
    std::string oid_;
    std::vector<int> oidArray_;
    bool isValid_;


    void stringToNumArray(std::string oid);
    void createSnmpItem(std::vector<uint8_t> &toSnmp, std::vector<uint8_t> &encodedOid);
    void decode(uint8_t *pData, int dataLen);
    // oid polozka je vzdy kladne cislo
    int64_t decode_multibytes(uint8_t *&pI, uint8_t *pEnd);


    ///
    /// \brief encode ... zakoduje do SNMP formatu oid
    /// \param objectId ... retezec, jednotlive ciselne polozky jsou oddeleny teckou '.'
    ///
    bool encode(std::string objectId, std::vector<uint8_t> &encodedOid);
    void encode_multibytes(int num, vector<uint8_t> &encodedOid);

public:
    SNMP_object();
    SNMP_object(const string &oid);
    virtual ~SNMP_object(){}

    void clear();
    bool isValid() const;
    std::string getValue(bool *isValid = NULL) const;
    void setValue(const string &oid);


    // SNMP_type interface
public:
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;
    virtual uint8_t getType() override;
    virtual int getItemSize() override;

    virtual std::string toString() override;
};

#endif // SNMP_OBJECT_H
