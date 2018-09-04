#ifndef SNMP_REQUEST_H
#define SNMP_REQUEST_H

#include "snmplib.h"
#include "types/snmpdata.h"
#include "snmpparser.h"
#include <list>

class SNMP_LIBRARY_EXPORT SNMP_ManagerRequest
{
    SNMP_ManagerRequest();

    static void createRequest(SnmpData &snmpData, SNMP_VERSION v, const std::string &community, const uint32_t &requestId)
    {
        snmpData.setVersion(v);
        snmpData.setCommunity(community);
        snmpData.setRequestId(requestId);
        snmpData.setErrorStatus(0);
        snmpData.setErrorIndex(0);
    }

    // vytvori varbindings pro snmpget
    static void fillRequestWithVarBindings(SnmpData &snmpData, const std::list<std::string> &oids)
    {
        for (auto &oid :oids)
        {
            snmpData.addVariable(oid, nullptr);
        }
    }

    static void fillRequestWithVarBindings(SnmpData &snmpData, std::list<VarBinding> &oids)
    {
        exit(0);
        for (auto &oid :oids)
        {
//            oid.getOid()->toString()
//            snmpData.addVariable(oid, NULL);
        }
    }

public:

    ///
    /// \brief createGetRequest ... vytvori get request pro seznam predanych OID
    /// \param snmpVersion
    /// \param community
    /// \param requestId
    /// \param oids
    /// \return
    ///
    static SnmpParser::snmpPacket createGetRequest(SNMP_VERSION snmpVersion, std::string community, uint32_t requestId, std::list<std::string> &oids);

    ///
    /// \brief createSetRequest
    /// \param snmpVersion
    /// \param community
    /// \param requestId
    /// \param oids
    /// \return
    ///
    static SnmpParser::snmpPacket createSetRequest(SNMP_VERSION snmpVersion, std::string community, uint32_t requestId, std::list<VarBinding> &oids);
};

#endif // SNMP_REQUEST_H
