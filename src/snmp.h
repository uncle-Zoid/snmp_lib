/**
 *
 * SnmpReader.h
 *
 * Class search for snmp packets and reads SET-REQUEST data for wanted oids.
 * Třída hleda snmp packet se SET-REQUEST a cte hodnoty pro danne oid.
 *
 * author: Dominik Papp dominik.papp@tcz.cz
 * (c) 2016 T-CZ a.s.
 *
**/


#ifndef SNMPREADER_H
#define SNMPREADER_H

#include <QObject>

#include <iostream>

#include <QString>
#include <QList>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>
#include <QDateTime>

struct sOIDDatax
{
        QString oid;
        uint intValue;
        QString stringValue;
        uint timestamp;

        bool hasString;
        bool hasOid;
        bool hasInt;
        bool hasNullPtr;

        sOIDDatax()
        {
            oid.clear();
            stringValue.clear();
            intValue = 0;
            timestamp = 0;
            hasInt=false;
            hasOid=false;
            hasNullPtr=false;
            hasString=false;
        }

        friend std::ostream &operator<<(std::ostream &os, sOIDDatax const &po)
        {
            if(!po.hasOid)
                return os;            

            os << QDateTime::fromTime_t(po.timestamp).toString("yyyy.MM.dd HH.mm.ss.zzz").toStdString() << ":  OID: " << po.oid.toStdString();
            if(po.hasInt)
                os << "\nintValue=" << po.intValue;
            if(po.hasString)
                os << "\nstringValue=" << po.stringValue.toStdString();
            if(po.hasNullPtr)
                os << "\ncontains nullPtr";

            os << std::endl;

            return os;
        }
};

struct sSNMPv1Info
{
    uchar error;
    uchar errorIndex;
    QString community;
    uint requestID;

    sSNMPv1Info()
    {
        error = 0;
        errorIndex = 0;
        community.clear();
        requestID = 0;
    }
};

class SnmpReader : public QObject
{
    Q_OBJECT
    static constexpr uchar SET_REQUEST = 0xA3;

    QTimer t;

private:
    QUdpSocket *pSocket;
    QList<QString> monitoringOids;
    int readLenght(const QByteArray &snmpPacket, int &actualPosition, uint *pLenBytesSize = NULL);
    uint readInt(const QByteArray &snmpPacket, int &actualPosition);
    QString readString(const QByteArray &snmpPacket, int &actualPosition);
    QString arrayToOID(const QByteArray &oidArr, int oidBegin, int oidLen);

public:
    explicit SnmpReader(QObject *parent = 0);

    void setPort(quint16 port);
    void addOid(QString oid);

signals:
    void oidValueAbailable(const struct sOIDDatax &data);
    void connectionState(bool isConnected);

private slots:
    void readyRead();

    void error(QAbstractSocket::SocketError err);

public slots:
    void disconnectUdp()
    {
        pSocket->close();
        emit connectionState(false);
    }
};

#endif // SNMPREADER_H
