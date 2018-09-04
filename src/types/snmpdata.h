#ifndef SNMPDATA_H
#define SNMPDATA_H

#include "varbinding.h"
#include "pdu.h"

#include <string>
#include <vector>
enum class SNMP_VERSION {V1 = 0, V2C = 1};
static const char *VERSION_TEXT[] = {"V1", "V2c"};
class SNMP_LIBRARY_EXPORT SnmpData
{
public:

private:
    using uint8_t = unsigned char;

    bool isValid_;
    SNMP_VERSION version_;
    uint8_t pdu_;
    int requestId_;
    int errorStatus_;
    int errorIndex_;
    std::string community_;
    shared_ptr<VarBindingList> varBindingsList_;

public:


    SnmpData()
        : isValid_(false)
        , version_(SNMP_VERSION::V1)
        , pdu_(0)
        , requestId_(0)
        , errorStatus_(0)
        , errorIndex_(0)
        , community_("")
        {}

    // ***************************
    // *** GETTERS and SETTERS ***
    // ***************************


    bool getIsValid() const
    {
        return isValid_;
    }
    void setIsValid(bool isValid)
    {
        isValid_ = isValid;
    }

    uint8_t getVersion() const
    {
        return uint8_t(version_);
    }
    const char* getVersionText() const
    {
        return VERSION_TEXT[int(version_)];
    }
    void setVersion(SNMP_VERSION version)
    {
        version_ = version;
    }
    uint8_t getPdu() const
    {
        return pdu_;
    }
    void setPduType(const uint8_t &pdu)
    {
        pdu_ = pdu;
    }
    int getRequestId() const
    {
        return requestId_;
    }
    void setRequestId(int requestId)
    {
        requestId_ = requestId;
    }
    int getErrorStatus() const
    {
        return errorStatus_;
    }
    void setErrorStatus(int errorStatus)
    {
        errorStatus_ = errorStatus;
    }
    int getErrorIndex() const
    {
        return errorIndex_;
    }
    void setErrorIndex(int errorIndex)
    {
        errorIndex_ = errorIndex;
    }
    std::string getCommunity() const
    {
        return community_;
    }
    void setCommunity(const std::string &community)
    {
        community_ = community;
    }
    shared_ptr<VarBindingList> getVarBindingsList() const
    {
        return varBindingsList_;
    }
    void setVarBindingsList(const shared_ptr<VarBindingList> &varBindingsList)
    {
        varBindingsList_ = varBindingsList;
    }



    // ***************************
    void addVariable(const string &oid, shared_ptr<SNMP_type> pValue = nullptr);
    void addVariable(const string &oid, void *pValue, uint8_t type);
    shared_ptr<SNMP_type> getVariable(const string &oid);


    string toString()const;
};

#endif // SNMPDATA_H
