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
 * @brief Расчет максимального времени передачи пакета по радиоэфиру
 * к пакету добавляется 12 байт заголовка + 2 байта CRC, 
 *      /12 байт в кадре
 *      + 1 - округление в большую сторону до целого числа кадров
 *      * 60 - в режиме 1/1 передача одного кадра из 12 байт осуществляется за 60 мс 
 *      + 60 мс на синхрокадр 
 *      + 120 мс запас на задержки обработки и в сети.
 * @param size - длина пакета
 * @return время в мс
 */
uint32_t calcTimeout(uint32_t size)
{
    return (((size + 12 + 2) / 12) + 1) * 60 + 60 + 120;
}
/**
 * @brief TrafficGenerator::start - Запуск обмена чере 1 сек.
 * @param target - получатель сообщения
 * @param group - признак того что получатель - группа.
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

    // Порядковый номер пакета для передачи, 0 должен пропускаться.
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

    // Максимальное количество пакетов для передачи, если 0 - не контролировать.
    if (m_count) {
        m_count--;
        if (!m_count)
            QCoreApplication::exit(0);
    }
}
/**
 * @brief TrafficGenerator::sendComplete
 * Событие что пакет полностью передан, здесь можно отправлять следующий пакет, 
 * но лучше подождать 60-120 мс, чтобы оставить время на передачу служебных данных по радиоканалу.
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
