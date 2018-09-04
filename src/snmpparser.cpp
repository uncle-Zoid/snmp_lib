#include <types/snmp_octetstring.h>
#include <cstring>
#include <QDebug>

#include "snmpparser.h"

snmp::itemTLV SnmpParser::readItem(uint8_t *&pData, const uint8_t *pDataEnd)
{
    snmp::itemTLV item;
    if((pData == pDataEnd) || (pData == NULL) || (pDataEnd == NULL))
    {
        item.pValue = NULL;
        return item;
    }
                                                      //                                                      |
    item.tag = *pData ++;                             //                                                      v
    item.length = LenghtCoder::decodeItemsLenght(pData, pDataEnd); //cte delku polozy a nastavi ukazatel na zacatek dat (TLV)

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
        item.length = 0;
    }

    return item;
}



SnmpData SnmpParser::parse(uint8_t *pData, int dataLen)       // zpracovani snmp packetu
{
    SnmpData data;
    processSnmp(pData, dataLen, data);
    return data;
}


void SnmpParser::processSnmpHeader(uint8_t *&pData, const uint8_t *pDataEnd, SnmpData &data)
{
//    printf("processSnmpHeader %p %d\n", pData, *pData);fflush(stdin);
    // verze snmp
    snmp::itemTLV item = readItem(pData, pDataEnd);
    SNMP_int snmpInt;
    if(!snmpInt.readFromSNMP(item))//item.pValue, item.length))
    {
        // chyba cteni verze snmp
        // FIXME throw "";
        throw "";
    }
    data.setVersion(static_cast<SNMP_VERSION>(snmpInt.getValue()));

    // community string
    item = readItem(pData, pDataEnd);
    SNMP_octetString snmpOctetString;
    snmpOctetString.readFromSNMP(item);
    data.setCommunity(snmpOctetString.getValue());
}

void SnmpParser::processSnmp(const uint8_t *pData, int dataLen, SnmpData &data)
{
//qDebug(" SnmpParser::processSnmp\tbegin");
    uint8_t *pDataTmp = (uint8_t*)pData;
    const uint8_t *pDataEnd = pDataTmp + dataLen;


    snmp::itemTLV item = readItem(pDataTmp, pDataEnd); // cti prvni item -> {0x30, packetLen, data}
    if(item.tag != TAG_SEQ)
    {
        data.setIsValid(false);
        return;
    }
    processSnmpHeader(item.pValue, pDataEnd, data);
    item = readItem(item.pValue, pDataEnd);

    Pdu pdu;
    pdu.readFromSNMP(item);
//qDebug()<<"SnmpParser::processSnmp, pdu_.getItemSize: " << pdu.getItemSize()<<endl;

    data.setIsValid(pdu.isValid());
    data.setPduType(pdu.getPduType());
    data.setRequestId(pdu.getRequestId());
    data.setErrorStatus(pdu.getErrorStatus());
    data.setErrorIndex(pdu.getErrorIndex());

    data.setVarBindingsList(pdu.getVarBindingList());
//qDebug(" SnmpParser::processSnmp\end");
}

SnmpParser::snmpPacket SnmpParser::toPacket(SnmpData &data)
{    
    SNMP_int si(data.getVersion());
    SNMP_octetString so(data.getCommunity());

    Pdu pdu;
    pdu.setPduType(data.getPdu());                  // PDU type
    pdu.setRequestId(data.getRequestId());      // request id
    pdu.setErrorStatus(data.getErrorStatus());  // error status
    pdu.setErrorIndex(data.getErrorIndex());    // error index
    pdu.setVarBindingList(data.getVarBindingsList());

    int totalWritten = 0;
    int maxPacketSize = si.getItemSize() + so.getItemSize() + pdu.getItemSize(); // velikost packetu bez 0x30 a delkyDat
    uint8_t buff[5];
    int lenoflen = LenghtCoder::encodeLenght(buff, 5, maxPacketSize);           // zakodovana delka dat v snmp
    maxPacketSize += LenghtCoder::getLenghtOfLenght(maxPacketSize) + 1/*snmp header 0x30*/; // celkova velikost snmp packetu
////    cout << "SnmpParser::toPacket, packet size: " << maxPacketSize << endl;

    snmpPacket packet;
    packet.pPacket = new uint8_t[maxPacketSize];
    memset(packet.pPacket, 0xcc, maxPacketSize); // NOTE, docasne, odstranit

    // zapis do snmp packetu
    uint8_t *pPos = packet.pPacket;
    *pPos ++ = TAG_SEQ;                 // 0x30
    memcpy(pPos, buff, lenoflen);       // snmp data size
    pPos += lenoflen;
                                        // snmp version
    try
    {
        int written = si.copyToSNMP(pPos, maxPacketSize - totalWritten);
        totalWritten += written;
        pPos += written;
                                            // snmp comunnity string
        written = so.copyToSNMP(pPos, maxPacketSize - totalWritten);
        totalWritten += written;
        pPos += written;

        pdu.copyToSNMP(pPos, maxPacketSize - totalWritten);
    }
    catch(const char * ex)
    {
        cout << ex << endl;
        return snmpPacket();
    }

    packet.isCorrect = true;
    packet.packetLen = maxPacketSize;

    return packet;
}

