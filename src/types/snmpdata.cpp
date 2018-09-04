#include <iostream>
#include <sstream>      // std::stringstream
#include <string.h>

#include "snmpdata.h"


shared_ptr<SNMP_type> SnmpData::getVariable(const string &oid)
{
    shared_ptr<VarBindingList> v = varBindingsList_;
    while(v)
    {
        if(v->vb->getOid()->toString() == oid)
        {
            return v->vb->getValue();
        }
        v = v->next;
    }
    return nullptr;
}


void SnmpData::addVariable(const string &oid, shared_ptr<SNMP_type> pValue)
{
    if(oid.empty())
        return;

    if(pValue == nullptr)
    {
        pValue = make_shared<SNMP_null>();
    }

    shared_ptr<VarBinding> vb = make_shared<VarBinding>();

    vb->setOid(make_shared<SNMP_object>(oid));
    vb->setValue(pValue);

    if(varBindingsList_ == nullptr)
    {
        varBindingsList_ = make_shared<VarBindingList>();
        varBindingsList_->vb = vb;
    }
    else
    {
        auto a = varBindingsList_;
        while(a->next)a=a->next;

        a->next = make_shared<VarBindingList>();
        a->next->vb = vb;
    }
}



void SnmpData::addVariable(const string &oid, void *pValue, uint8_t type)
{
    if(oid.empty())
        return;

    shared_ptr<SNMP_type> share;
    switch(type)
    {
        case TAG_INT:
            share = make_shared<SNMP_int>(*((int*)pValue));
        break;
    }

    shared_ptr<VarBinding> vb = make_shared<VarBinding>();

    vb->setOid(make_shared<SNMP_object>(oid));
    vb->setValue(share);

    if(varBindingsList_ == nullptr)
    {
        varBindingsList_ = make_shared<VarBindingList>();
        varBindingsList_->vb = vb;
    }
    else
    {
        auto a = varBindingsList_;
        while(a->next)a=a->next;

        a->next = make_shared<VarBindingList>();
        a->next->vb = vb;
    }
}



string SnmpData::toString() const
{
    stringstream str;
    str << "---------------------------------------------------------------";
    str << "\nSnmp version: " << getVersionText();
    str << "\nSnmp community: " << getCommunity();
    str << "\nSnmp PDU: " << hex << (int)getPdu();
    str << "\nSnmp request id: " << dec << getRequestId();
    str << "\nsnmp error-status: "  << dec << getErrorStatus();
    str << "\nSnmp error-index: " << dec << getErrorIndex();
//    str << "\nSnmp items: (" << getVariables().size() << ")";

    shared_ptr<VarBindingList> a = getVarBindingsList();
    while(a)
    {
        str << "\n\t*********************************************";
        str << "\n\tOid: " << a->vb->getOid()->toString();
        str << "\n\tValue [" << (int)a->vb->getValue()->getType() << "]: " << a->vb->getValue()->toString() << "\t\tisValid = "<<(a->vb->isValid()?"true":"false");

        a = a->next;
    }
    str << "\n---------------------------------------------------------------\n";
    return str.str();
}
