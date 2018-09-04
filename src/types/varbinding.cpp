#include <cstring>

#include "varbinding.h"
#include "../snmpparser.h"
#include "snmp_octetstring.h"
#include "snmp_null.h"
#include "snmp_boolean.h"
#include "snmp_uint32.h"
#include "snmp_ipaddress4.h"
#include "snmp_netsnmpinvalidoid.h"

VarBinding::VarBinding()
{
    value_ = NULL;
    oid_ = NULL;
    clear();
    initTable();
}

//VarBinding::VarBinding(string oid, SNMP_type *value)
//{
//    setOid(oid);
//    setValue(value);
//}

//VarBinding::VarBinding(std::shared_ptr<SNMP_object> &oid, std::shared_ptr<SNMP_type> &value)
//{
//    setOid(oid);
//    setValue(value);
//}

//VarBinding::VarBinding(string oid, shared_ptr<SNMP_type> value)
//{
//    oid_ = make_shared<SNMP_object>(oid);
//    value_ = (value == NULL) ? make_shared<SNMP_null>() : value;
//}

//VarBinding::VarBinding(const std::shared_ptr<SNMP_object> &oid, shared_ptr<SNMP_type> value)
//{
//    oid_ = oid;
//    value_ = (value == NULL) ? make_shared<SNMP_null>() : value;
//}

void VarBinding::initTable()
{
    m_[TAG_NULL]         = &VarBinding::readNull;
    m_[TAG_INT]          = &VarBinding::readInt;
    m_[TAG_OCTET_STRING] = &VarBinding::readOctetString;
    m_[TAG_OBJ]          = &VarBinding::readObject;
    m_[TAG_BOOLEAN]      = &VarBinding::readBoolean;
    m_[TAG_UNSIGNED]     = &VarBinding::readUnsigned;
    m_[TAG_IP]           = &VarBinding::readIp4;
    m_[TAG_NETSNMP_INVALID_OID] = &VarBinding::readInvalidOid;
}
void VarBinding::readObject(snmp::itemTLV &item)
{
    oid_ = make_shared<SNMP_object>();
    oid_->readFromSNMP(item);
    isValidOid_ = dynamic_cast<SNMP_object *>(oid_.get())->isValid();
}
void VarBinding::readInt(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_int>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_int *>(value_.get())->isValid();
}
void VarBinding::readUnsigned(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_uint32>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_uint32 *>(value_.get())->isValid();
}
void VarBinding::readOctetString(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_octetString>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_octetString *>(value_.get())->isValid();
}
void VarBinding::readNull(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_null>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_null *>(value_.get())->isValid();
}
void VarBinding::readBoolean(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_boolean>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_boolean *>(value_.get())->isValid();
}
void VarBinding::readIp4(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_ipAddress4>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_ipAddress4 *>(value_.get())->isValid();
}
void VarBinding::readInvalidOid(snmp::itemTLV &item)
{
    value_ = make_shared<SNMP_netsnmpInvalidOid>();
    value_->readFromSNMP(item);
    isValidValue_ = dynamic_cast<SNMP_netsnmpInvalidOid *>(value_.get())->isValid();
}

bool VarBinding::isValid() const
{
    return oid_->isValid() & isValidValue_;
}

std::shared_ptr<SNMP_object> VarBinding::getOid()
{
    return oid_;//oid_.getValue();
}

std::shared_ptr<SNMP_type> VarBinding::getValue()
{
    return value_;
}


void VarBinding::setOid(std::shared_ptr<SNMP_object> oid)
{    
    if(oid == NULL) oid_ = make_shared<SNMP_object>();
    else             oid_ = oid;
}

void VarBinding::setValue(std::shared_ptr<SNMP_type> value)
{
    if (value == NULL)  value_ = make_shared<SNMP_null>();
    else                value_ = value;
}

void VarBinding::clear()
{
    oid_ = NULL;
    value_ = NULL;
    isValidOid_ = false;
    isValidValue_ = false;
}

int VarBinding::copyToSNMP(uint8_t *pData, int dataLen)
{
    // sekvence {0x30, L, {0x06, L_OID, V_OID}, {T_value, L_value, V_value}}

    if(oid_ == NULL || value_ == NULL || pData == NULL)
    {
        throw "VarBinding::copyToSNMP() ... oid, value, nebo pData  je NULL!!!";
    }

    int oidSize   = oid_->getItemSize();
    int valueSize = value_->getItemSize();

    if(oidSize < 0 || valueSize < 0)
    {
        throw "VarBinding::copyToSNMP() ... oid nebo value ma neplatnou delku!!!";
    }

    // zakoduj delku vb
    uint8_t bufferValuesLenght[5];
    int L = LenghtCoder::encodeLenght(bufferValuesLenght, 5, oidSize + valueSize);


    int toWrite = oidSize + valueSize + L + 1;

    if(toWrite > dataLen) // data se nevejdou
    {
        throw "VarBinding::copyToSNMP() ... nedostate mista v packetu!!!";
    }

    // zde musi byt overeno, ze se sekvence do snmp packetu vejde!!!
    *pData ++ = TAG_SEQ;                    // T
    memcpy(pData, bufferValuesLenght, L);   // L
    pData += L;                             //
    dataLen = dataLen - L - 1;
                                            // V
    // OID
    int written = oid_->copyToSNMP(pData, dataLen);
    if(written < 0) throw "VarBinding::copyToSNMP() ... zde MUSI byt v bufferu dost dat pro zapis. snmp packet nyni bude poskozeny!!!";
    pData += written;
    dataLen -= written;

    // VALUE
    written = value_->copyToSNMP(pData, dataLen);
    if(written < 0) throw "VarBinding::copyToSNMP() ... zde MUSI byt v bufferu dost dat pro zapis. snmp packet nyni bude poskozeny!!!";

    return toWrite;
}

bool VarBinding::readFromSNMP(snmp::itemTLV &item)
{
    // v sekvenci musi byt VZDY prvne oid a pak hodnota
    if(item.tag != TAG_SEQ || item.pValue == nullptr)
    {
        return false;
    }
    const uint8_t *pDataEnd = item.pValue + item.length;
    uint8_t *pData = item.pValue;

    // vycte a vytvori polozku 'oid' typu SNMP_object
    snmp::itemTLV subitem = SnmpParser::readItem(pData, pDataEnd);
    readObject(subitem);

    // vycte a vytvori polozku 'value' typu SNMP_type
    subitem = SnmpParser::readItem(pData, pDataEnd);
    auto it = m_.find(subitem.tag);
    if(it != m_.end())
    {
        (this->*it->second)(subitem); // vycte a vytvori polozku 'value' daneho typu
    }
    else // typ me neznami
    {
        value_ = make_shared<SNMP_null>();
        isValidValue_ = false;
    }


    return isValidValue_;
}

uint8_t VarBinding::getType()
{
    return TAG_SEQ;
}

int VarBinding::getItemSize()
{
    if(oid_ == NULL || value_ == NULL)
        return -1;

    int oidSize = oid_->getItemSize();
    int valueSize = value_->getItemSize();

    if(oidSize  < 0 || valueSize < 0)
        return -1;

    int L = LenghtCoder::getLenghtOfLenght(oidSize+valueSize);
    return (1) + (L) + (oidSize + valueSize); //sizeof(T) + sizeof(L) + sizeof(V)
}
