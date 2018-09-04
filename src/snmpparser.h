#ifndef SNMPPARSER_H
#define SNMPPARSER_H

#include <inttypes.h>
#include "snmplib.h"
#include "types/pdu.h"
#include "types/lenghtcoder.h"
#include "types/types.h"
#include "types/snmp_int.h"
#include "types/snmpdata.h"

class SNMP_LIBRARY_EXPORT SnmpParser
{
    friend class Pdu;
    friend class VarBinding;

public:
    // datova polozka
    struct snmpPacket
    {
        snmpPacket()
            : packetLen(0)
            , pPacket(NULL)
            , isCorrect(false)
            { }

        int packetLen;
        uint8_t *pPacket;
        bool isCorrect;
    };

    SnmpParser()
    {}

    SnmpData parse(uint8_t *pData, int dataLen);
    SnmpParser::snmpPacket toPacket(SnmpData &data);

protected:

    ///
    /// \brief Snmpv1::readItem ... cte polozku z snmp packetu
    /// \param pData    ... zacatek dat pro polozku, po dokonceni cteni pe tento ukazatel posunut na dalsi polozku
    /// \param pDataEnd ... ukazuje jeden bajt za posledni platny datovy bajt
    /// \return ... vraci strukturu udavajici pozici, typ a velikost predchazejici polozky
    ///
    static snmp::itemTLV readItem(uint8_t *&pData, const uint8_t *pDataEnd);

private:
    void processSnmpHeader(uint8_t *&pData, const uint8_t *pDataEnd, SnmpData &data);
    void  processSnmp(const uint8_t *pData, int dataLen, SnmpData &data);

};

#endif // SNMPPARSER_H
