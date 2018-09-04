#include "snmp_manager_request.h"

#include <QDebug>
SNMP_ManagerRequest::SNMP_ManagerRequest()
{

}

SnmpParser::snmpPacket SNMP_ManagerRequest::createGetRequest(SNMP_VERSION snmpVersion, string community, uint32_t requestId, std::list<string> &oids)
{    
    SnmpData snmpData;
    createRequest(snmpData, snmpVersion, community, requestId);
    fillRequestWithVarBindings(snmpData, oids);
    snmpData.setPduType(PDU_GET);

    SnmpParser parser;
    return parser.toPacket(snmpData);
}

SnmpParser::snmpPacket SNMP_ManagerRequest::createSetRequest(SNMP_VERSION snmpVersion, string community, uint32_t requestId, std::list<VarBinding> &oids)
{
    SnmpData snmpData;
    createRequest(snmpData, snmpVersion, community, requestId);
    fillRequestWithVarBindings(snmpData, oids);
    snmpData.setPduType(PDU_SET);

    SnmpParser parser;
    return parser.toPacket(snmpData);
}
