#ifndef SNMPV1_H
#define SNMPV1_H

#include "types/lenghtcoder.h"

#include <QVariant>

class Snmpv1
{
public:
    static constexpr int TAG_INT = 0x02;
    static constexpr int TAG_STRING = 0x04;
    static constexpr int TAG_SEQ = 0x30;
    static constexpr int TAG_OBJECT = 0x06;

    static constexpr int PDU_GET      = 0xA0;
    static constexpr int PDU_GET_NEXT = 0xA1;
    static constexpr int PDU_RESPONSE = 0xA2;
    static constexpr int PDU_SET      = 0xA3;
    static constexpr int PDU_TRAP     = 0xA4;

    static constexpr int NON_VALID_TAG = -1;


    // datova polozka
    typedef struct
    {
        int valueType;
        QString oid;
        QVariant value;        
    }snmpDataItem;

    // snmp
    typedef struct SNMP_DATA_ITEM
    {
        SNMP_DATA_ITEM()
            : isValid(false)
            , version(0)
            , pdu(0)
            , community("")
            , requestId(0)
            , errorStatus(0)
            , errorIndex(0)
            {}
        bool isValid;
        uint8_t version;    	// pritomno vzdy
        uint8_t pdu;
        QString community;      // pritomno vzdy
        int requestId;
        int errorStatus;
        int errorIndex;

        QList<snmpDataItem> variables;

    }snmpData;

private:
    typedef struct
    {
        uint8_t tag;
        int length;
        uint8_t *pValue;
    }item;

    using type=QVariant (Snmpv1::*)(const item &);
    std::map <int, type> m_;

    void initTable()
    {
//        m_[0x06] = &decodeObject;
        m_[0x04] = &decodeString;
        m_[0x02] = &decodeInt;
        m_[0x06] = &decodeObject;
//        m_.insert(std::pair<int, type>( 0x04, &Snmpv1::decodeString));

//        m_.emplace(0x06, &decodeObject);
    }



    ///
    /// \brief readItemLenght
    /// \param *data ... ukazuje na prvni bajt velikosti
    /// \return
    ///
    int decodeItemsLenght(uint8_t *&data, const uint8_t *dataEnd);
    int decodeItemsLenght_multibytes(uint8_t *&data, int remainingData);
    int decodeItemsLenght_singlebytes(uint8_t *&data, int remainingData);


    QVariant decodeObject(const item &item); //QString
    QVariant decodeString(const item &item); //QString
    QVariant decodeInt(const item &item);    //int
    snmpDataItem decodeSequence(const item &item);

    bool processPDU(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem);


    item readItem(uint8_t *&pData, const uint8_t *pDataEnd);
    bool readSnmpVersion(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem);
    bool readCommunity(uint8_t *&data, const uint8_t *dataEnd, snmpData &snmpItem);
    int readInt(uint8_t *&data, const uint8_t *dataEnd, bool &ok);
    bool readVarBindingList(uint8_t *&pData, const uint8_t *pDataEnd, snmpData &snmpItem);

public:
    Snmpv1();
    snmpData processSnmp(const uint8_t *pData, int dataLen);

};

#endif // SNMPV1_H
