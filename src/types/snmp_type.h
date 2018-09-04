#ifndef BER_INTERFACE_H
#define BER_INTERFACE_H

#include "snmplib.h"
#include <cstdint>
#include <string>
//using uint8_t=unsigned char;
//#include "types/types.h"
namespace snmp {
    struct itemTLV;
}

class SNMP_LIBRARY_EXPORT SNMP_type
{
public:
    SNMP_type(){}


    virtual ~SNMP_type(){}
    ///
    /// \brief copyToSNMP
    /// \param pData
    /// \param dataLen
    /// \return
    ///
    virtual int copyToSNMP(uint8_t *pData, int dataLen) = 0;

    ///
    /// \brief readFromSNMP
    /// \param pData
    /// \param dataLen
    /// \return
    ///
    virtual bool readFromSNMP(uint8_t *pData, int dataLen) {(void)pData; (void)dataLen; return false; }

    ///
    /// \brief readFromSNMP
    /// \param item
    /// \return
    ///
    virtual bool readFromSNMP(snmp::itemTLV &item) = 0;

    ///
    /// \brief getType
    /// \return
    ///
    virtual uint8_t getType() = 0;

    ///
    /// \brief getItemSize
    /// \return
    ///
    virtual int getItemSize() = 0;
//    virtual //polozku item pro cvaceni ze cteni

    virtual std::string toString() = 0;
};

#endif // BER_INTERFACE_H
