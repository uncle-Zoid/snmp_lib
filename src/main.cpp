#include <QCoreApplication>
#include <iostream>

#include <QUdpSocket>

using namespace std;

#include <bitset>

#include "types/ber.h"
#include "types/snmp_octetstring.h"
#include "types/snmp_int.h"
#include "types/snmp_uint32.h"
#include "types/snmp_null.h"
#include "types/snmp_object.h"
#include "types/snmp_boolean.h"
#include "snmpparser.h"
#include "snmp_manager_request.h"
// Example program
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>

#include <chrono>
#include <list>


int main()
{
    SnmpData data;
    data.setVersion(SNMP_VERSION::V2C);
    data.setCommunity("public");
    data.setPduType(PDU_GET);
    data.addVariable("1.3.6.1.4.1.53002.31.1.4.1.0");
//    data.addVariable("1.3.6.1.4.1.53002.31.1.4.2.0");
//    data.addVariable("1.3.6.1.4.1.53002.31.1.1.0");
    data.addVariable("1.3.6.1.4.1.53002.31.1.14.0");

    SnmpParser p;
    auto a = p.toPacket(data);

    QUdpSocket socket;
    socket.writeDatagram((const char *)a.data, a.size, QHostAddress::Broadcast, 161);

    while(socket.bytesAvailable() <= 0);

    char datax[1024];
    auto re = socket.readDatagram(datax, 1024);

    cout << "bajtu: " << re << endl << "data: ";
    for(int i = 0; i < re; ++i)
    {
        printf("%02X ", uint8_t(datax[i]));
    }
    cout << endl;

    SnmpParser pp;
    auto aaa = pp.parse((uint8_t *)datax, re);

    cout << "***************** PRIJATO *****************\n" << aaa.toString() << endl;


    auto var = aaa.getVariable("1.3.6.1.4.1.53002.31.1.4.1.0");
    if(var)
    {
        cout << "VAR=" << int(var->getType()) << "   " << var->toString() << endl;
    }

    SNMP_object oid("1.3.6.1.4.1.53002.31.1.4.1.0");
    cout << (*aaa.getVarBindingsList()->vb->getOid().get() == oid) << endl;
    return 0;
}

int x()
{
    std::list<std::string> ll;


//    const int drOperatingStatus_oid[] 		= { 1,3,6,1,4,53002,1,1 };
//    const int drReceiverSensitivity_oid[] 	= { 1,3,6,1,4,53002,1,2 };
//    const int drTimeSourceState_oid[] 		= { 1,3,6,1,4,53002,1,3 };
//    const int drGpsTicks_oid[] 				= { 1,3,6,1,4,53002,1,4 };
//    const int drMessages_oid[] 				= { 1,3,6,1,4,53002,1,5 };
//    const int drSmodMessages_oid[] 			= { 1,3,6,1,4,53002,1,6 };
//    const int drSmodCorrectMessages_oid[] 	= { 1,3,6,1,4,53002,1,7 };
//    const int drTimerOverflows_oid[] 		= { 1,3,6,1,4,53002,1,8 };
//    const int drCycles_oid[] 				= { 1,3,6,1,4,53002,1,9 };
//    const int drFw_oid[] 					= { 1,3,6,1,4,53002,1,10 };
//    const int drHw_oid[] 					= { 1,3,6,1,4,53002,1,11 };

    ll.push_back("1.3.6.1.4.53002.2.2.0");
//    ll.push_back("1.3.6.1.4.53002.1.2.0");
//    ll.push_back("1.3.6.1.4.53002.1.3.0");
    SnmpParser::snmpPacket pac  = SNMP_ManagerRequest::createGetRequest(SNMP_VERSION::V1, "public", 0, ll);

    for(uint8_t *pp = pac.data; pp != pac.data+pac.size; ++pp)
    {
        printf("%02X", (int)*pp);
    }
cout<<endl;

    uint8_t buuu[] = {0x30, 0x29, 0x02, 0x01, 0x01, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6C, 0x69, 0x63, 0xA2, 0x1C, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x11, 0x30, 0x0F, 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x83, 0x9E, 0x0A, 0x02, 0x02, 0x00, 0x02, 0x01, 0xEE, };
    SnmpParser bee;
    cout << bee.parse(buuu, sizeof(buuu)).toString();

            ;


    auto t = chrono::high_resolution_clock::now();
    auto t2 = chrono::high_resolution_clock::now();


    cout << endl << "Doba itoa: \t" << chrono::duration_cast<chrono::nanoseconds>(t2 - t).count() << " ns, "
         << (chrono::duration_cast<chrono::microseconds>(t2 - t)).count() << " us, "
         << chrono::duration_cast<chrono::milliseconds>(t2 - t).count() << " ms" << endl;




}

