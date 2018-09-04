#include "snmp_object.h"
#include "types/types.h"

using namespace std;

SNMP_object::SNMP_object()
{
    clear();
}

SNMP_object::SNMP_object(const string &oid)
{
    clear();
    setValue(oid);
}


bool SNMP_object::isValid() const
{
    return isValid_;
}

void SNMP_object::clear()
{
    isValid_ = false;
    oid_.clear();
    encodedOid_.clear();
}

string SNMP_object::getValue(bool *isValid) const
{
    if(isValid != NULL)
        *isValid = isValid_;
    return oid_;
}

void SNMP_object::setValue(const string &oid)
{
    oid_ = oid;
    stringToNumArray(oid);
//    isValid_ = encode(oid_, encodedOid_);
//    createSnmpItem(toSnmp_, encodedOid_);
}


void SNMP_object::decode(uint8_t *pData, int dataLen)
{
    if(*pData != 0x2B)// v snmp VZDY zacina oid 1.3 == 0x2B
    {
        cout<<"!!!NEPLATNE OID!!!\n";
        isValid_ = false;
        return;
    }

    oid_="1.3";
    // oid bajt >= 128 (0x80) vicebajtove vyjadreni cisla ... zahrnuje vsechny nasledujici bajty ktere jsou < 0x80 vcetne
    //projdi datove bajty
    uint8_t *pEnd = pData + dataLen;
    int64_t number;
    for (uint8_t *pI = pData + 1; pI != pEnd; )
    {
        if (*pI & 0x80) // multibajtove zakodovane id
        {
            number = decode_multibytes(pI, pEnd);
            if(number < 0)
            {
                isValid_ = false;
                return;
            }
        }
        else
        {
            number = *pI ++;//oid += QString().sprintf(".%d", *pI++);
        }

        oid_.append("." + std::to_string(number));
    }
    isValid_ = true;

    stringToNumArray(oid_);
}
int64_t SNMP_object::decode_multibytes(uint8_t *&pI, uint8_t *pEnd)
{
    uint32_t number = 0;
    while(*pI & 0x80)
    {
        if(pI == pEnd)
        {
            cout << "Snmpv1::decodeObject(item &item)... narazeno na konec dat pri dekodovani oid multibytes.";
            return -1;
        }
        number |= (*pI & 0x7F);
        number <<= 7;
        ++ pI;
    }
    number |= *pI & 0x7F;
    ++ pI;

    return number;
}


bool SNMP_object::encode(string objectId, vector<uint8_t> &encodedOid)
{
    if(objectId[0]=='.')
        objectId.erase(0,1);

    if(objectId.substr(0, 4) == "1.3.")
    {
        encodedOid.clear();
//        objectId.erase(0, 4);
    }
    else
    {
//        isValid_ = false;
        return false;
    }

    encodedOid.push_back(0x2b);

    // projdi retezec po cislech
    size_t pos = 4;
    size_t posLast = 4;

    while(pos != std::string::npos)
    {
        pos = objectId.find('.', posLast);
        int num = atoi(objectId.substr(posLast, pos-posLast).data()); //cout << "pos: "<<pos << " lastpos: "<<posLast<<",   "<<str.substr(posLast, pos-posLast) << endl;
        posLast = pos+1;

        if(num > 127) // cislo vetsi 127 se koduje do vice bajtu
        {
            encode_multibytes(num, encodedOid);
        }
        else
        {
            encodedOid.push_back(uint8_t(num));
        }
    }
    return true;
}
void SNMP_object::encode_multibytes(int num, vector<uint8_t> &encodedOid)
{
    uint8_t oid[4]={0};

    // oid = {LSB, ... , MSB}
    int index = 0;
    while(num)
    {
        oid[index] = num % 128;

        if(index > 0)
            oid[index] |= 0x80;

        num >>= 7; // /= 128;
        ++ index;
    }

    for(int i = index - 1; i >=0 ; --i)
        encodedOid.push_back(oid[i]);

}

void SNMP_object::stringToNumArray(string oid)
{
    //cout<< "SNMP_object::stringToNumArray: ";
    oidArray_.clear();

    if(oid[0]=='.')
        oid.erase(0,1);

    size_t pos = 0;
    size_t posLast = 0;
    while(pos != std::string::npos)
    {
        pos = oid.find('.', posLast);
        int num = atoi(oid.substr(posLast, pos-posLast).data());
        oidArray_.push_back(num);
        posLast = pos+1;
        //cout<<num << " ";
    }
    //cout<<endl;
}

void SNMP_object::createSnmpItem(std::vector<uint8_t> &toSnmp, std::vector<uint8_t> &encodedOid)
{
    uint8_t buffer[5];
    int lenlen = LenghtCoder::encodeLenght(buffer, 5, encodedOid.size());

    toSnmp.clear();
    toSnmp.push_back(TAG_OBJ);
    toSnmp.insert(toSnmp.end(), buffer, buffer + lenlen);
    toSnmp.insert(toSnmp.end(), encodedOid.begin(), encodedOid.end());
}


// SNMP_type interface


int SNMP_object::copyToSNMP(uint8_t *pData, int dataLen)
{
    isValid_ = encode(oid_, encodedOid_);
    createSnmpItem(toSnmp_, encodedOid_);

    if(toSnmp_.size() > size_t(dataLen))
    {
        return -1;
    }

    for(auto &a : toSnmp_)
        *pData++ = a;

    return toSnmp_.size();
}


bool SNMP_object::readFromSNMP(uint8_t *pData, int dataLen)
{
    // 0x06
    if(pData == NULL || *pData != TAG_OBJ)
    {
        isValid_ = false;
        return false;
    }

    const uint8_t *pDataEnd = pData + dataLen;
    int toRead = LenghtCoder::decodeItemsLenght(pData, pDataEnd);
    if(toRead > dataLen)
    {
        isValid_ = false;
        return false;
    }

    decode(pData, toRead);

    return isValid_;
}


bool SNMP_object::readFromSNMP(snmp::itemTLV &item)
{
    if(item.pValue == NULL || item.tag != TAG_OBJ)
    {
        isValid_ = false;
        return false;
    }
    decode(item.pValue, item.length);
    return isValid_;
}


uint8_t SNMP_object::getType()
{
    return TAG_OBJ;
}


int SNMP_object::getItemSize()
{

//    if(objectId.substr(0, 4) == "1.3.")
//    {
//        encodedOid.clear();
////        objectId.erase(0, 4);
//    }
//    else
//    {
////        isValid_ = false;
//        return false;
//    }

//    encodedOid.push_back(0x2b);

//    // projdi retezec po cislech
//    size_t pos = 4;
//    size_t posLast = 4;

//    while(pos != std::string::npos)
//    {
//        pos = objectId.find('.', posLast);
//        int num = atoi(objectId.substr(posLast, pos-posLast).data()); //cout << "pos: "<<pos << " lastpos: "<<posLast<<",   "<<str.substr(posLast, pos-posLast) << endl;
//        posLast = pos+1;

//        if(num > 127) // cislo vetsi 127 se koduje do vice bajtu
//        {
//            int index = 0;
//            while(num)
//            {
//                oid[index] = num % 128;

//                if(index > 0)
//                    oid[index] |= 0x80;

//                num >>= 7; // /= 128;
//                ++ index;
//            }
//        }
//        else
//        {
//            encodedOid.push_back(uint8_t(num));
//        }
//    }

    vector<uint8_t> v1,v2;
    bool isval = encode(oid_, v1);
    if(isval)
        createSnmpItem(v2, v1);

    return isval ? v2.size() : (-1);
}

string SNMP_object::toString()
{
    return oid_;
}

