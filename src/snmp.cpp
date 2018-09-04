#include "snmp.h"

SnmpReader::SnmpReader(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<sOIDDatax>("sOIDDatax");

    pSocket = new QUdpSocket(this);

}

void SnmpReader::setPort(quint16 port)
{
    if(pSocket)
    {
        if(pSocket->isOpen())
            pSocket->close();

        bool isBinded = pSocket->bind(QHostAddress::Any, port);//QHostAddress::Any,
        emit connectionState(isBinded);

         QObject::connect(pSocket, &QUdpSocket::readyRead, this, &SnmpReader::readyRead);
         //QObject::connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    }
    else
        emit connectionState(false);
}

void SnmpReader::error(QAbstractSocket::SocketError err)
{
    qDebug()<<pSocket->errorString();
    //emit connectionState(false);//TODO
}

void SnmpReader::addOid(QString oid)
{
    monitoringOids.append(oid);
}

void SnmpReader::readyRead()
{
    QByteArray snmpPacket;

    while(pSocket->hasPendingDatagrams())
    {
        sSNMPv1Info snmp;

        snmpPacket.resize(pSocket->pendingDatagramSize());
        pSocket->readDatagram(snmpPacket.data(), snmpPacket.size());


        int actualPosition=0;
        if(snmpPacket.size()>0)
        {
//            qDebug()<<"data prijata="<<QDateTime().currentDateTime().toString("HH:mm:ss.zzz");
            int dataLen=0;
            uint byteCount = 0;

//            qDebug()<<"prijato dat="<<snmpPacket.size()<<" od:"<<host<<" "<<port;
//            qDebug()<<"\tData="<<snmpPacket.left(30).toHex() << "="<<snmpPacket.left(30);

            if(snmpPacket.size() < 3)
            {
                qDebug()<<"  !!!ERROR!!! malo dat";
                continue;
            }

            /// zracovani
            // 1. over hlavicku
            if((uchar)snmpPacket[actualPosition] != 0x30) // SNMPv1 vzdy zacina bajtem 0x30
            {
                qDebug()<<"  !!!ERROR!!! neni SNMP"<<actualPosition<<" "<<snmpPacket[actualPosition];
                continue;
            }
            ++actualPosition;

            // 2. over velikost datoveho bloku
            dataLen = readLenght(snmpPacket, actualPosition, &byteCount);
//            qDebug()<<"velikost dat  packetu="<<dataLen;
            if((uint)snmpPacket.size() < dataLen+byteCount)
            {
                qDebug()<<"  !!!ERROR!!! malo dat v packetu!!!";
                continue;
            }

            // 3. overeni verze SNMP
            if((uchar)snmpPacket[actualPosition] != 0x02)
            {
                qDebug()<<"  !!!ERROR!!!  verze neni snmp v1 (neni typu int)";
                continue;
            }
            ++ actualPosition;
            //if(!(snmpPacket[actualPosition] == 0x02 && snmpPacket[actualPosition+1]==0x01 && snmpPacket[actualPosition+2] == 0x00))// snmp v1
            if(readInt(snmpPacket, actualPosition) != 0)
            {
                qDebug()<<"  !!!ERROR!!! neni SNMPv1";
                continue;
            }

            // 4. precti comunity string
            if((uchar)snmpPacket[actualPosition] != 0x04) // nasleduje typ string
            {
                qDebug("  !!!ERROR!!!  nenasleduje community string");
                continue;
            }
            ++ actualPosition;
            snmp.community = readString(snmpPacket, actualPosition);
//            qDebug()<<communityString;

            // 5. request type ... dale zpracovavat jen SET_REQUEST
            if((uchar)snmpPacket[actualPosition] != SET_REQUEST) // chci cist jen setRequest
            {
                //qDebug()<<QString().sprintf("neni setRequest zprava=%X",(uchar)snmpPacket[actualPosition]);
                continue;
            }
//            qDebug()<<QString().sprintf("%X",(uchar)snmpPacket[actualPosition]);
            ++actualPosition;


            // 6. zjisti velikost dat
            dataLen = readLenght(snmpPacket, actualPosition, &byteCount);
//            qDebug()<<"dataLen"<<dataLen;

            // 7. precti request ID
            if((uchar)snmpPacket[actualPosition]!=0x02)
            {
                qDebug()<<"  !!!ERROR!!!  request ID";
                continue;
            }
            ++ actualPosition;
//            qDebug()<<"request ID "<<
            snmp.requestID = readInt(snmpPacket ,actualPosition);

            // 8. precti error
            if((uchar)snmpPacket[actualPosition] != 0x02)// && snmpPacket[actualPosition+1]==0x01))
            {
                qDebug()<<"  !!!ERROR!!!  zpracovani erroru";
                continue;
            }
            ++ actualPosition;
            snmp.error = readInt(snmpPacket, actualPosition);// snmpPacket[actualPosition+2];

            // 9. precti error index // TODO co je erorindex
            if((uchar)snmpPacket[actualPosition] != 0x02)// && snmpPacket[actualPosition+1]==0x01))
            {
                qDebug()<<"  !!!ERROR!!!  zpracovani error Index";
                continue;
            }
            ++ actualPosition;
            snmp.errorIndex = readInt(snmpPacket, actualPosition);// snmpPacket[actualPosition+2];
//            qDebug()<<"error="<<error<<" ,errorIndex="<<errorIndex;
//            qDebug("Zacatek dat: Typ=%X",(uchar)snmpPacket[actualPosition]);

            /// 10. zde pokracuji data v sekvencich
            if((uchar)snmpPacket[actualPosition] == 0x30) //sekvence
            {
//                qDebug()<<"data do kone: "<<snmpPacket.mid(actualPosition).toHex();
                ++actualPosition;

                /// precti celou sekvenci s daty
                auto len= readLenght(snmpPacket, actualPosition, &byteCount);
                QByteArray datapart = snmpPacket.mid(actualPosition, len);
                int datapartSize = datapart.size();
//                qDebug()<<"@@@ dataSize="<<ds<<", actualIndes="<<actualPosition<<"\n\t"<<datapart.toHex();

                int index=0;
                /// projdi pole s daty
                while(datapart.size() > 0)
                {
                    char type = datapart[index];
//                    qDebug()<<"typ="<<type;
                    ++index;

                    int l = readLenght(datapart, index, &byteCount);

//                    qDebug()<<"velikost casti="<<l;

                    struct sOIDDatax dato;
                    switch((uchar)type)
                    {
                    case 0x30:
                        // zacatek dat ... index+byteCount
                        // konec dat ... l
                        QByteArray datoArr = datapart.mid(index, l);
//                        qDebug()<<"DATO="<<datoArr.toHex();
                        while(datoArr.size()>0)
                        {
                            int offset=0;
                            switch((uchar)datoArr[offset]) // datovy typ
                            {
                            case 0x06:
                            {
                                ++offset;
                                int l = readLenght(datoArr, offset, &byteCount);
                                QByteArray aoid = datoArr.mid(offset, l);
//                                qDebug()<<"OID="<<aoid.toHex();
                                dato.oid = arrayToOID(aoid, 0, l);
                                dato.hasOid = true;
                                offset += l;
                                break;
                            }
                            case 0x02:
                            {
                                ++offset;
                                dato.hasInt = true;
                                dato.intValue = readInt(datoArr, offset);
                                break;
                            }
                            case 0x04:
                            {
                                dato.hasString = true;
                                dato.stringValue = readString(datoArr, offset);
                                break;
                            }
                            case 0x05:
                            {
                                dato.hasNullPtr=true;
//                                qDebug()<<"null value";
                            }
                            default:
                                qDebug()<<"!!!nerozpoznany druh dat!!!";
                                datoArr.clear();
                            }
                            datoArr.remove(0, offset);
                        }
                        break;

//                    case 0x06:
//                        break;
                    }

                    // pokud je oid v seznamu, emit
                    foreach(auto oid, monitoringOids)
                    {
                        if(oid == dato.oid)
                        {                            
                            dato.timestamp = QDateTime::currentDateTime().toTime_t();
//                            qDebug()<<"Vlozena casova znacka="<<oid<<" "<<QDateTime::fromTime_t(dato.timestamp).toString("HH:mm:ss.zzz")<<"\n";
                            emit oidValueAbailable(dato);
//                            std::cout<<dato<<std::endl;
                        }
                    }
                    datapart.remove(0, l+byteCount+1);
//                    qDebug()<<"dataSize="<<ds<<"\t"<<datapart.toHex();
                    index=0;
                }

                actualPosition+=datapartSize;

            }
            else
            {
                qDebug()<<"neni sekvence, zpracuj jikak";
            }
        }//end if
    }

}

//#include <QVariant>
//#include <map>

//typedef struct
//{
//    QVariant value;
//    uint8_t *newPosition;

//}reti;

//map <int, void> m_;
//void initTable()
//{

//}

//// cteni sekvence ... sekvence obsahuje
//reti readSequence(uint8_t *data)
//{
//    reti v;
//    v.value =
//}


//void processSnmp(uint8_t *data, int dataLen)
//{

//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// \brief readLenght ... cte z predaneho pole od dane pozice Lenght
/// \param snmpPacket
/// \param actualPosition ... aktualni pozice v packetu
/// \param pLenBytesSize ... vraci pocet bajtu obsahujicich udaj o delce
/// \return
///
int SnmpReader::readLenght(const QByteArray &snmpPacket, int &actualPosition, uint *pLenBytesSize)
{
    if (snmpPacket.size()<1)
        return -1;
    // actualPosition ukazuje na bajt delkaDat
    int number=0;
    if(snmpPacket[actualPosition] & 0x80)// pokud bajt velikosti je >127, velikost je udana ve vide bajtech
    {
        // pr. 0x82 0x02 0x9e   => 0x29e == 670
        int lenght = snmpPacket[actualPosition] & 0x7F; // +1 protoze prvni bajt nese nfo o poctu bajtu
        ++ actualPosition; //posun  ukazovatka na zacatek dat

        if (snmpPacket.size()<actualPosition+lenght)
            return -1;

        for(int i = 0; i < lenght; ++i)
        {
            number += uchar(snmpPacket[actualPosition + i]) << 8*(lenght - i - 1);
        }

        actualPosition += lenght;

        if(pLenBytesSize)
            *pLenBytesSize=lenght+1;
    }
    else
    {
        number = snmpPacket[actualPosition];
        ++ actualPosition;

        if(pLenBytesSize)
            *pLenBytesSize=1;
    }

    return number;
}

uint SnmpReader::readInt(const QByteArray &snmpPacket, int &actualPosition)
{
    int lenght = readLenght(snmpPacket, actualPosition);
    uint number = 0;

    if (snmpPacket.size()<actualPosition+lenght)
        return UINT_MAX;

    for(int i = 0; i < lenght; ++i)
    {
        number += uchar(snmpPacket[actualPosition + i]) << 8*(lenght - i - 1);
    }

    actualPosition += lenght;
    return number;
}

QString SnmpReader::readString(const QByteArray &snmpPacket, int &actualPosition)
{
    int lenght = readLenght(snmpPacket, actualPosition);
    if (snmpPacket.size()<actualPosition+lenght)
        return "";
    QString str = snmpPacket.mid(actualPosition, lenght);

    actualPosition += lenght;

    return str;
}

QString SnmpReader::arrayToOID(const QByteArray &oidArr, int oidBegin, int oidLen)
{
    QString oid;
    if(oidArr[0] != 0x2B)
    {
        qDebug()<<"!!!NENI OID!!!";
        return "OID_ERR";
    }

    oid = "1.3";
    //oidArr[0] == 0x2B vzdy => 1.3
    uint subNumber=0;
    for (int i = oidBegin+1; i < oidLen; )
    {
        if(oidArr[i]&0x80)//oid cislo je > 127
        {
            int endLargeNumberIndex = i;
            while(oidArr[++ endLargeNumberIndex] & 0x80); // najdi konec (prvni bajt, ktery nezacina 1xxx xxxx)(ALE PATRI DO VYPOCTU CISLA)

            int largeNumLenght=endLargeNumberIndex-i+1;

            if(oidArr.size() < (i + largeNumLenght))
            {
                qDebug()<<"Error parsing OID";
                return "OID_ERR";
            }
            subNumber = 0;
            for (int ii = 0; ii < largeNumLenght; ++ii)
                subNumber |= uchar(oidArr[i + ii] & 0x7F) << 7*(largeNumLenght-ii-1);

            i += largeNumLenght;
        }
        else
        {
            subNumber = oidArr[i];
            ++i;
        }

        oid += QString().sprintf(".%d", subNumber);
    }
    return oid;
}
