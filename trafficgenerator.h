#ifndef TRAFFICGENERATOR_H
#define TRAFFICGENERATOR_H

#include <QObject>
#include <QTimer>
#include "dtranms_proto.h"
#include "dtranmsinstance.h"

class TrafficGenerator : public QObject
{
    Q_OBJECT

public:
    TrafficGenerator(QObject *parent,
                     const QString &host,
                     uint16_t port_src,
                     uint16_t port_dst,
                     uint8_t SID,
                     DTRANMS::eFEC FEC,
                     uint16_t size,
                     uint32_t timeout,
                     uint32_t count);
    void start(DMRID target, bool group);

protected slots:
    void sendPacket();
    void sendComplete(uint8_t DN);
    void receiveData(DMRID source, uint8_t SID, uint8_t DN, QByteArray data);

protected:
    DtranMsInstance *m_dtran;
    uint8_t m_SID;
    DTRANMS::eFEC m_FEC;
    uint16_t m_size;
    uint32_t m_timeout;
    uint32_t m_count;
    DMRID m_target;
    bool m_group;
    QTimer m_timer;
    uint8_t m_fsnDn = 0;
    uint8_t m_sndDn = 0;
    uint64_t m_timestamp;
};

#endif // TRAFFICGENERATOR_H
