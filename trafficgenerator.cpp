#include "trafficgenerator.h"
#include <QCoreApplication>

void log(const char *format, ...);
void info(const char *format, ...);
uint64_t timestamp_ms();

TrafficGenerator::TrafficGenerator(QObject *parent,
                                   const QString &host,
                                   uint16_t port_src,
                                   uint16_t port_dst,
                                   uint8_t SID,
                                   DTRANMS::eFEC FEC,
                                   uint16_t size,
                                   uint32_t timeout,
                                   uint32_t count)
    : QObject(parent)
    , m_SID(SID)
    , m_FEC(FEC)
    , m_size(size)
    , m_timeout(timeout)
    , m_count(count)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), SLOT(sendPacket()));
    m_dtran = new DtranMsInstance(this, port_src, QHostAddress(host), port_dst);
    connect(m_dtran, SIGNAL(sendComplete(uint8_t)), SLOT(sendComplete(uint8_t)));
    connect(m_dtran,
            SIGNAL(receiveDLP(DMRID, uint8_t, uint8_t, QByteArray)),
            SLOT(receiveData(DMRID, uint8_t, uint8_t, QByteArray)));
}

/**
 * @brief ������ ������������� ������� �������� ������ �� ����������
 * � ������ ����������� 12 ���� ��������� + 2 ����� CRC, 
 *      /12 ���� � �����
 *      + 1 - ���������� � ������� ������� �� ������ ����� ������
 *      * 60 - � ������ 1/1 �������� ������ ����� �� 12 ���� �������������� �� 60 �� 
 *      + 60 �� �� ���������� 
 *      + 120 �� ����� �� �������� ��������� � � ����.
 * @param size - ����� ������
 * @return ����� � ��
 */
uint32_t calcTimeout(uint32_t size)
{
    return (((size + 12 + 2) / 12) + 1) * 60 + 60 + 120;
}
/**
 * @brief TrafficGenerator::start - ������ ������ ���� 1 ���.
 * @param target - ���������� ���������
 * @param group - ������� ���� ��� ���������� - ������.
 */
void TrafficGenerator::start(DMRID target, bool group)
{
    m_timeout = qMax(m_timeout, calcTimeout(m_size));
    m_target = target;
    m_group = group;

    info("Start sending to %u, group=%s, timeout=%u ms", target, group ? "Y" : "N", m_timeout);

    m_dtran->getGatewayState();

    m_timer.start(1000);
}
void TrafficGenerator::sendPacket()
{
    uint8_t data[m_size];
    memset(data, 0, sizeof(data));

    if (m_sndDn != m_fsnDn) {
        info("Timeout expired for packet %d", m_fsnDn, m_fsnDn);
    }

    // ���������� ����� ������ ��� ��������, 0 ������ ������������.
    m_fsnDn++;
    if (!m_fsnDn)
        m_fsnDn++;

    if (m_dtran->sendDlpPacket(m_target, m_group, m_FEC, m_SID, m_fsnDn, data, m_size)) {
        m_timestamp = timestamp_ms();
        m_timer.start(qMax(m_timeout, calcTimeout(m_size)));
    } else {
        info("ERROR: Send packet");
        QCoreApplication::exit(-1);
    }

    // ������������ ���������� ������� ��� ��������, ���� 0 - �� ��������������.
    if (m_count) {
        m_count--;
        if (!m_count)
            QCoreApplication::exit(0);
    }
}
/**
 * @brief TrafficGenerator::sendComplete
 * ������� ��� ����� ��������� �������, ����� ����� ���������� ��������� �����, 
 * �� ����� ��������� 60-120 ��, ����� �������� ����� �� �������� ��������� ������ �� �����������.
 */
void TrafficGenerator::sendComplete(uint8_t DN)
{
    m_sndDn = DN;
    if (m_sndDn == m_fsnDn) {
        info("Packet %d sent in %d ms", DN, timestamp_ms() - m_timestamp);
    }
    if (!m_timeout)
        m_timer.start(120);
}
void TrafficGenerator::receiveData(DMRID source, uint8_t SID, uint8_t DN, QByteArray data)
{
    info("Received packet from %u, SID=%u, DN=%d, len=%d", source, SID, DN, data.size());
}
