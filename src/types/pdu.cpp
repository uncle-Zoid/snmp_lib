#include "pdu.h"


#include "../snmpparser.h"
#include "types/varbinding.h"
#include <QDebug>

Pdu::Pdu()
    : isValid_(false)
    , varBindingList_(NULL)
    {}

bool Pdu::isValid() const
{
    return isValid_;
}


void Pdu::clear()
{
    varBindingList_ = NULL;
    isValid_ = 0;
    pduType_= 0;;
    requestId_ = 0;;
    errorStatus_ = 0;;
    errorIndex_ = 0;;
}

uint8_t Pdu::getPduType() const
{
    return pduType_;
}
void Pdu::setPduType(const uint8_t &pdu)
{
    pduType_ = pdu;
}

int Pdu::getRequestId() const
{
    return requestId_;
}
int Pdu::getErrorStatus() const
{
    return errorStatus_;
}

void Pdu::setErrorStatus(int errorStatus)
{
    errorStatus_ = errorStatus;
}
void Pdu::setRequestId(int requestId)
{
    requestId_ = requestId;
}

int Pdu::getErrorIndex() const
{
    return errorIndex_;
}
void Pdu::setErrorIndex(int errorIndex)
{
    errorIndex_ = errorIndex;
}

shared_ptr<VarBindingList> Pdu::getVarBindingList() const
{
    return varBindingList_;
}
void Pdu::setVarBindingList(shared_ptr<VarBindingList> vb)
{
    varBindingList_ = vb;
}


bool Pdu::readSnmpPDU(snmp::itemTLV &item)
{
//    qDebug(" Pdu::readSnmpPDU\tbegin");
    clear();// ctu, zrus shared_ptr na varbindings, budu vytvaret novy

    // PDU | LEN | id | err | err | var-binding-list
    if(item.pValue == nullptr)
        return false;

    const uint8_t *pDataEnd = item.pValue + item.length;
    uint8_t *pData = item.pValue;

    pduType_ = item.tag;
    bool readStatus=false;
    switch(pduType_)
    {
        case PDU_GET:
        case PDU_GET_NEXT:
        case PDU_RESPONSE:
        case PDU_SET:
            readStatus |= readPduHeader(pData, pDataEnd);
            readStatus |= readVarBindingList(pData, pDataEnd);
        break;

        case PDU_TRAP:
        break;

        default:
            throw "Pdu::readSnmpPDU: neznami typ PDU\n";
    }
    return readStatus;
}

bool Pdu::readPduHeader(uint8_t *&pData, const uint8_t *pDataEnd)
{
    SNMP_int si;
    // precti ID
    snmp::itemTLV pdu = SnmpParser::readItem(pData, pDataEnd);
    if(pdu.pValue == nullptr)
    {
        return false; // malo dat
    }
    si.readFromSNMP(pdu);
    requestId_ = si.getValue();

    // precti error-status
    pdu = SnmpParser::readItem(pData, pDataEnd);
    if(pdu.pValue == nullptr)
    {
        return false; // malo dat
    }
    si.readFromSNMP(pdu);
    errorStatus_ = si.getValue();

    // precti error-index
    pdu = SnmpParser::readItem(pData, pDataEnd);
    if(pdu.pValue == nullptr)
    {
        return false; // malo dat
    }
    si.readFromSNMP(pdu);
    errorIndex_ = si.getValue();
    isValid_ = true;
    return true;
}

bool Pdu::readVarBindingList(uint8_t *&pData, const uint8_t *pDataEnd)
{
    // 0x30 LL {0x30 L OID VALUE}
    if(pData == nullptr || pDataEnd == nullptr)
        return false;

    int packetSize = pDataEnd - pData;

    // precti velikost varbindinglist
    auto item = SnmpParser::readItem(pData, pDataEnd);  // musi byt typ sekvence

    if(item.pValue == nullptr || item.tag != TAG_SEQ || item.length > packetSize)
    {
        cout << "Snmpv1::readVarBindingList: chyba ve var binding list: T=" << item.tag << "L=" <<item.length <<"/"<<packetSize<<"V=" << item.pValue <<endl;

//        FIXME throw "Snmpv1::readVarBindingList: chyba ve var binding list:";
        throw "Snmpv1::readVarBindingList: chyba ve var binding list:";
    }

    uint8_t *pVarbindList_begin = item.pValue;
    const uint8_t *pVarbindList_end = item.pValue + item.length;
    while(pVarbindList_begin != pVarbindList_end)
    {
        shared_ptr<VarBinding> vb = make_shared<VarBinding>();
        // {0x30} {len} {0x06 len oid} {typ len value}
        auto subitem = SnmpParser::readItem(pVarbindList_begin, pVarbindList_end); //vyctu sub-sekvenci
        try
        {
            if(vb->readFromSNMP(subitem))
            {

                shared_ptr<VarBindingList> vbl = make_shared<VarBindingList>();
                vbl->vb = vb;

                if(varBindingList_ == nullptr)
                    varBindingList_ = vbl;
                else
                {
                    auto end = varBindingList_;
                    while(end->next) end=end->next;

                    end->next = vbl;
                }
            }
        }
        catch(const char *ex)
        {
            cout << ex << endl;
        }
    }

    return true;
}



int Pdu::copyToSNMP(uint8_t *pData, int dataLen)
{
    // potrebuju znat celou delku varbindinglist
    int vblTotalLen= getVariableBindingListSize();
    if(vblTotalLen < 0)
    {
        throw "LOGUJ, Pdu::copyToSNMP, invalid varBindingList length";
        return -1; // chyba ve var-binding list
    }
    uint8_t encodedVBLsize[5];
    int vblEncodedLenLen = LenghtCoder::encodeLenght(encodedVBLsize, 5, vblTotalLen);

    SNMP_int siId(requestId_);
    SNMP_int siErrs(errorStatus_);
    SNMP_int siErr(errorIndex_);

    // zapsat tag pdu a velikost pdu
    int pduTotalLen = 1 + vblEncodedLenLen + vblTotalLen + siId.getItemSize() + siErr.getItemSize() + siErrs.getItemSize();
    uint8_t encodedPDUsize[5];
    int pduEncodedLenLen = LenghtCoder::encodeLenght(encodedPDUsize, 5, pduTotalLen);

    *pData = pduType_;
    memcpy((pData + 1), encodedPDUsize, pduEncodedLenLen);               // L

    int totalWritten = 1 + pduEncodedLenLen;
    int written = siId.copyToSNMP(pData + totalWritten, dataLen - totalWritten);
    if(written < 0)
    {
        cout << "Pdu::copyToSNMP: nelze zapsat" << endl;
        return -1;
    }
    totalWritten += written;

    written = siErrs.copyToSNMP(pData + totalWritten, dataLen - totalWritten);
    if(written < 0)
    {
        cout << "Pdu::copyToSNMP: nelze zapsat" << endl;
        return -1;
    }
    totalWritten += written;

    written = siErr.copyToSNMP(pData + totalWritten, dataLen - totalWritten);
    if(written < 0)
    {
        cout << "Pdu::copyToSNMP: nelze zapsat" << endl;
        return -1;
    }
    totalWritten += written;
    //pData += totalWritten;

    // nakopirovat var-binding list
    *(pData + totalWritten) = TAG_SEQ;                                              // T
    ++ totalWritten;
    memcpy((pData + totalWritten), encodedVBLsize, vblEncodedLenLen);               // L
    totalWritten += vblEncodedLenLen;                                               //
                                                                                    // V
    auto a = varBindingList_;
    while(a)
    {
        int written = a->vb->copyToSNMP(pData + totalWritten, dataLen - totalWritten);
        if(written < 0)
        {
            throw "Pdu::copyToSNMP ... LOGUJ, chyba pri zapisu varbindings do snmp";
        }
        totalWritten += written;

        a = a->next;
    }

    return totalWritten;
}


bool Pdu::readFromSNMP(snmp::itemTLV &item)
{
    return readSnmpPDU(item);//item.pValue, item.pValue + item.length);
}

uint8_t Pdu::getType()
{
    return pduType_;
}

int Pdu::getItemSize()
{    
    int length = 0;//3x typ int
    SNMP_int si(requestId_);
    length += si.getItemSize();
    si.setValue(errorStatus_);
    length += si.getItemSize();
    si.setValue(errorIndex_);
    length += si.getItemSize();

    int vblLen = getVariableBindingListSize();
    int vblLenLen = LenghtCoder::getLenghtOfLenght(vblLen); // velikost velikosti vbl
    if(vblLen < 0)
        return vblLen;


    length += vblLen + vblLenLen + 1/*0x30*/;
    int pduLen = LenghtCoder::getLenghtOfLenght(length); /*velikost datove casti PDU*/
    length += pduLen + 1; /* +1 = PDU_TAG*/

    return length;
}

int Pdu::getVariableBindingListSize()
{
    int vblTotalLen=0;
    auto vbl = varBindingList_;
    while(vbl)
    {
        int len = vbl->vb->getItemSize();
        if(len < 0)
            return len;

        vblTotalLen += len;

        vbl = vbl->next;
    }
    return vblTotalLen;
}
