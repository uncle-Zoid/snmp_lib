#ifndef VARBINDING_H
#define VARBINDING_H

#include "snmp_type.h"
#include "snmp_object.h"
#include "snmp_null.h"
#include "types.h"
#include "lenghtcoder.h"
//#include "../snmpparser.h"

#include <string>
#include <map>

///
/// \brief The VarBinding class ... trida vytvari z nastavenych promennych oid a value 'SNMP varbinding' (0x30 | L | OID | value), kterou zapise do snmp packetu-
///                                 nebo cte ze snmp packetu varbinding a poskytuje hodnoty OID a value
///
class SNMP_LIBRARY_EXPORT VarBinding : public SNMP_type
{
    std::shared_ptr<SNMP_object> oid_;//SNMP_object oid_;
    std::shared_ptr<SNMP_type> value_;
    bool isValidOid_;
    bool isValidValue_;

    using type = void (VarBinding::*)(snmp::itemTLV &);
    std::map <int, type> m_;

    void initTable();

    void readInt(snmp::itemTLV &item);
    void readOctetString(snmp::itemTLV &item);
    void readObject(snmp::itemTLV &item);
    void readNull(snmp::itemTLV &item);
    void readBoolean(snmp::itemTLV &item);
    void readUnsigned(snmp::itemTLV &item);
    void readIp4(snmp::itemTLV &item);
    void readInvalidOid(snmp::itemTLV &item);

public:
    VarBinding();
//    VarBinding(string oid, SNMP_type *value);
//    VarBinding(std::shared_ptr<SNMP_object> &oid, std::shared_ptr<SNMP_type> &value);
//    VarBinding(string oid, shared_ptr<SNMP_type> value = NULL);
//    VarBinding(const std::shared_ptr<SNMP_object> &oid, shared_ptr<SNMP_type> value=NULL);

    void clear();
    bool isValid() const;
    //std::string getOid();
    std::shared_ptr<SNMP_object> getOid();
    std::shared_ptr<SNMP_type> getValue();
//    void setOid(std::string &oid);
    void setOid(std::shared_ptr<SNMP_object> oid);
//    void setValue(int &value);
    void setValue(std::shared_ptr<SNMP_type> value);
//    void setValue(SNMP_type *value);
//    void setValue(std::string &value);
//    void setValue(bool &value);


    // SNMP_type interface
public:
    ///
    /// \brief copyToSNMP ... vytvari sub-sekvenci a vlozi ji na konec snmp packetu
    /// \param pData ... kam vlozit v snmp pacteku
    /// \param dataLen ... kolik v packetu zbyva mista
    /// \return
    ///
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;

    ///
    /// \brief readFromSNMP
    /// \param item ... pozaduje na vstupu polozku typu sekvence
    /// \return
    ///
    virtual bool readFromSNMP(snmp::itemTLV &item) override;

    virtual uint8_t getType() override;
    virtual int getItemSize() override;

    virtual std::string toString() override{return "";} //NOTE ... VarBinding::toString

};



#endif // VARBINDING_H
