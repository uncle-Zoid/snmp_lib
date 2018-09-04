#ifndef PDU_H
#define PDU_H

#include "snmp_type.h"
#include "types.h"
#include "snmp_int.h"
#include "varbinding.h"

#include <vector>
#include <list>

//class SnmpParser;
struct VarBindingList
{
    VarBindingList()
        : vb  (nullptr)
        , next(nullptr)
    {}

    std::shared_ptr<VarBinding> vb;
    shared_ptr<VarBindingList> next;
};


///
/// \brief The Pdu class ... trida obsahuje informace z PDU a polozky z varbindinglist ve forme map<oid, value>
///
class Pdu : public SNMP_type
{    
    bool isValid_;
    uint8_t pduType_;
    int requestId_;
    int errorStatus_;
    int errorIndex_;
//    std::vector<snmp::itemVarBinding> oids_;
//    std::vector<VarBinding> varBindingList_;
    bool readVarBindingList(uint8_t *&pData, const uint8_t *pDataEnd);
    bool readSnmpPDU(snmp::itemTLV &item);


    shared_ptr<VarBindingList> varBindingList_; // NOTE new version --- predelat na std::list

public:
    Pdu();
    bool isValid() const;
    void clear();
    uint8_t getPduType() const;
    void setPduType(const uint8_t &pdu);
    int getRequestId() const;
    int getErrorStatus() const;
    void setErrorStatus(int errorStatus);
    void setRequestId(int requestId);
    int getErrorIndex() const;
    void setErrorIndex(int errorIndex);
    shared_ptr<VarBindingList> getVarBindingList() const;
    void setVarBindingList(shared_ptr<VarBindingList> vb);

    // SNMP_type interface
public:
    virtual int copyToSNMP(uint8_t *pData, int dataLen) override;
    virtual bool readFromSNMP(snmp::itemTLV &item) override;
    virtual uint8_t getType() override;
    virtual int getItemSize() override;

    virtual std::string toString() override{return "";}//NOTE .. Pdu::toString


private:
    bool readPduHeader(uint8_t *&pData, const uint8_t *pDataEnd);
    int getVariableBindingListSize();
};

#endif // PDU_H
