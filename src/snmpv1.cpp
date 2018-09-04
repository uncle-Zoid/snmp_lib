#include "snmpv1.h"

#include <map>
#include <iostream>

using namespace std;

Snmpv1::Snmpv1()
{
    initTable();
}


int Snmpv1::decodeItemsLenght(uint8_t *&data, const uint8_t *dataEnd)
{
    int remainingData = distance((const uint8_t *)data, dataEnd);

    // delka polozky muze byt v jednom, nebo vice bajtech
    return ((*data) & 0x80)
            ? decodeItemsLenght_multibytes(data, remainingData)
            : decodeItemsLenght_singlebytes(data, remainingData);
}
int Snmpv1::decodeItemsLenght_multibytes(uint8_t *&data, int remainingData)
{
    if(remainingData <= 0)
    {
        return -1; // neni dost dat
    }

    if(((*data) & 0x7F) == 0) // nespec. delka polozky, polozka konci typen EOC
    {
        ++ data;
        return 0;
    }

    // delka je vicebajtova ... *(data) = n=pocet bajtu delky, *(data+1)...*(data+n)=delka polozky v bajtech
    int len = (*data) & 0x7F;
    if (len > (int)sizeof(int) || len > remainingData) // kdyz bude delka ve vic jak 4 bajtech,
    {
        cout << "Velikost datove polozky je ulozena ve vice jak 4 bajtech. s tim si zatim neporadim";
//        data += len;
        return -1; // chyba cteni, polozka ma moc velkou delku
    }

    int value = 0;
    for (int i = 1; i <= len; ++i)
    {
        value |= *(data + i) << 8*(len - i); // delka ulozena v big-endian
    }
    data += len + 1;// ukazuje na dalsi polozku, nebo hodnotu polozky
    return value;
}
int Snmpv1::decodeItemsLenght_singlebytes(uint8_t *&data, int remainingData)
{
    if(remainingData <= 0)
    {
        return -1; // neni dost dat
    }

    return *data++;
}


///
/// \brief Snmpv1::readItem
/// \param pData
/// \param pDataEnd ... ukazuje jeden bajt za posledni platny datovy bajt
/// \return
///
Snmpv1::item Snmpv1::readItem(uint8_t *&pData, const uint8_t *pDataEnd)
{
    item item;
    if((pData == pDataEnd) || (pData == NULL) || (pDataEnd == NULL))
    {
        item.pValue = NULL;
        return item;
    }
                                                      //                                                      |
    item.tag = *pData ++;                             //                                                      v
    item.length = decodeItemsLenght(pData, pDataEnd); //cte delku polozy a nastavi ukazatel na zacatek dat (TLV)

    if(item.length >= 0) // pokud byla delka dat polozky zakodovana ve vic jak 4 bajtech, neumim precist
    {
//        cout << "readItem distance: " << dec << distance((const uint8_t *)pData, pDataEnd) << endl;
        //pData nyni ukazuje na VALUE polozky, overit, ze je dostatek bajtu pro cteni
        if (distance((const uint8_t *)pData, pDataEnd) < item.length) // protoze pDataEnd ukazuje za posledni prvek v poli, vraci distance delku platneho pole
        {
            cout << "Snmpv1::readItem: neni dostatek dat pro polozku " << (int)item.tag << ", ktera vyzaduje " << item.length << " bajtu" << endl;
            item.pValue = NULL;
        }
        else
        {
            item.pValue = pData;
            pData += item.length; // pretoc ukazatel v datech na dalsi polozku
        }
    }
    else
    {
        item.pValue = NULL; // chyba cteni, neni dost dat, nebo nedekoduju delku
    }

    return item;
}


Snmpv1::snmpData Snmpv1::processSnmp(const uint8_t *pData, int dataLen)
{    
    uint8_t *pDataTmp = (uint8_t*)pData;
    uint8_t *dataEnd = pDataTmp + dataLen;
    snmpData snmpItem;


    // prvni polozka v snmp musi byt 0x30
    item item = readItem(pDataTmp, dataEnd);
    if(item.tag != 0x30)//neni snmp
    {
       return snmpItem;
    }
//    int snmpPacketLen = item.length; // zbyvajici velikost dat
    if(item.pValue == NULL) // neni dostatek dat !!!  // if(snmpPacketLen > (dataLen - 2)) // (dataLen - 2) -> do delky snmpPacketu neni zahrnuha hlavicka 0x30 a udaj o velikosti
    {
        cout << "Snmpv1::processSnmp: Chyba, prectena velikost je vetsi, nez veliost packetu" << endl;
        return snmpItem;
    }
    dataEnd = pDataTmp; // data ukazuje nyni na konec snmp packetu

    // druha polozka v snmp musi byt verze
    if(!readSnmpVersion(item.pValue, dataEnd, snmpItem))
    {
        cout << "Snmpv1::processSnmp: chyba pri zpracovani verze" << endl;
        return snmpItem;
    }

    // treti polozka musi byt community
    if(!readCommunity(item.pValue, dataEnd, snmpItem))
    {
        cout << "Snmpv1::processSnmp: chyba pri zpracovani community" << endl;
        return snmpItem;
    }

    // pote nasleduje PDU, nebo TRAP
    if(!processPDU(item.pValue, dataEnd, snmpItem))
    {
        cout << "Snmpv1::processSnmp: chyba PDU" << endl;
        return snmpItem;
    }

    snmpItem.isValid = true;
    return snmpItem;
}

bool Snmpv1::processPDU(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem)
{
    // *data == pdu type
    // *(data+1) == pdu len

    auto pduItem = readItem(data, dataEnd); // bude obsahovat PDU
    snmpItem.pdu = pduItem.tag; // pdu-typ

    if(snmpItem.pdu == PDU_GET || snmpItem.pdu == PDU_GET_NEXT || snmpItem.pdu == PDU_RESPONSE || snmpItem.pdu == PDU_SET)
    {
        // request id
        bool ok=false;
        snmpItem.requestId = readInt(pduItem.pValue, dataEnd, ok);
        if(!ok)return false;

        // error status
        snmpItem.errorStatus = readInt(pduItem.pValue, dataEnd, ok);
        if(!ok)return false;

        // error index
        snmpItem.errorIndex = readInt(pduItem.pValue, dataEnd, ok);
        if(!ok)return false;


        //polozky v sekvenci ... sekvence|delka| { {sekvence|delka|OID|VAL|} {sekvence|delka|IOD|VAL} ... }
        return readVarBindingList(pduItem.pValue, dataEnd, snmpItem);
    }
    // TODO process TRAP !! pozor pokud TRAP, bude mit jiny format !!!
    return true;
}

bool Snmpv1::readVarBindingList(uint8_t *&pData, const uint8_t *pDataEnd, snmpData &snmpItem)
{
    if(pData == NULL || pDataEnd == NULL)
        return false;

    // nacti velikost varbindinglist
    auto item = readItem(pData, pDataEnd);  // musi byt typ sekvence
    if(item.tag != TAG_SEQ || item.pValue == NULL)
    {
        cout << "Snmpv1::readVarBindingList: chyba ve var binding list" << endl;
        return false;
    }

    uint8_t *pVarbindList_begin = item.pValue;
    const uint8_t *pVarbindList_end = item.pValue + item.length;
    do
    {
        // {0x30 len} {0x06 len oid} {typ len value}
        auto subitem = readItem(pVarbindList_begin, pVarbindList_end); //vyctu sub-sekvenci
        if(subitem.tag == TAG_SEQ)
        {
            // dekoduj subsekvenci
            auto di = decodeSequence(subitem);
            if(di.valueType != NON_VALID_TAG)
            {
                snmpItem.variables.append(di);
            }
        }
    }while(pVarbindList_begin != pVarbindList_end);

    return true;
}

bool Snmpv1::readCommunity(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem)
{
    auto item = readItem(data, dataEnd);
    if(item.tag == TAG_STRING && item.pValue != NULL)
    {
        snmpItem.community = decodeString(item).toString();
        return true;
    }
    return false;
}

bool Snmpv1::readSnmpVersion(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem)
{
//    auto item = readItem(data, dataEnd);
//    if(item.tag == TAG_INT)
//    {
//        snmpItem.version = decodeInt(item);
//        return true;
//    }
    bool ok=false;
    int version = readInt(data, dataEnd, ok);
    if(ok) snmpItem.version = version;
    return ok;
}

int Snmpv1::readInt(uint8_t *&data, const uint8_t *dataEnd, bool &ok)
{
    auto item = readItem(data, dataEnd);
    if(item.tag == TAG_INT && item.pValue != NULL)
    {
        ok=true;
        return decodeInt(item).toInt();
    }
    ok=false;
    return 0;
}


Snmpv1::snmpDataItem Snmpv1::decodeSequence(const Snmpv1::item &item)
{
    snmpDataItem di;
    di.valueType = NON_VALID_TAG; // neplatny typ, neplatna polozka!!!
    if(item.pValue == NULL)
    {
        return di;
    }
    uint8_t * pSeqData_begin = item.pValue; // zacatek subsekvence
    const uint8_t *pSeqData_end = item.pValue + item.length; // konec subsekvence

    // v sekvenci musi byt VZDY prvne oid
    auto subitem = readItem(pSeqData_begin, pSeqData_end);  // precti objekt s oid
    if(subitem.pValue == NULL || subitem.tag != TAG_OBJECT)
    {
        cout << "Snmpv1::decodeSequence: chyba nacitani oid." << endl;
        return di;
    }
    di.oid = decodeObject(subitem).toString();

    // cti a zpracuj hodnotu
    subitem = readItem(pSeqData_begin, pSeqData_end);  // precti objekt s hodnotou
    if(subitem.pValue == NULL)
    {
        cout << "Snmpv1::decodeSequence: chyba nacitani polozky." << endl;
        return di;
    }
    auto it = m_.find(subitem.tag);
    if(it != m_.end())
    {
        di.value = (this->*it->second)(subitem);
    }
    di.valueType = subitem.tag;

    return di;
}

QVariant Snmpv1::decodeInt(const Snmpv1::item &item)
{
    if(item.length > 5)
    {
        ;
    }
    int value = 0;
    for (int i = 0; i < item.length; ++i)
    {
        value |= *(item.pValue + i) << (item.length - i - 1);
    }
    return value;
}

QVariant Snmpv1::decodeString(const Snmpv1::item &item)
{
    return QByteArray((char*)item.pValue, item.length);
}

QVariant Snmpv1::decodeObject(const Snmpv1::item &item)
{
    if(item.pValue == NULL)
    {
        return "";
    }

    QString oid="1.3";
    if(item.pValue[0] != 0x2B)// v snmp vzdy zacina oid 1.3 == 0x2B
    {
        cout<<"!!!NENI OID!!!\n";
        return oid;
    }

    // oid bajt >= 128 (0x80) vicebajtove vyjadreni cisla ... zahrnuje vsechny nasledujici bajty ktere jsou < 0x80 vcetne
    //projdi datove bajty
    uint8_t *pEnd = item.pValue + (item.length);
    for (uint8_t *pI = item.pValue + 1; pI != pEnd; )
    {
        if (*pI & 0x80) // multibajtove zakodovane id
        {
            uint32_t number = 0;
            while(*pI & 0x80)
            {
                if(pI == pEnd)
                {
                    cout << "Snmpv1::decodeObject(item &item)... narazeno na konec dat pri dekodovani oid multibytes.";
                    return "";
                }
                number |= (*pI & 0x7F);
                number <<= 7;
                ++ pI;
            }
            number |= *pI & 0x7F;
            ++ pI;
            oid += QString().sprintf(".%d", number);
        }
        else
        {
            oid += QString().sprintf(".%d", *pI++);
        }
    }
    return oid;
}

