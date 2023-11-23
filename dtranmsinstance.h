#ifndef DTRANMSINSTANCE_H
#define DTRANMSINSTANCE_H

#include <QByteArray>
#include <QUdpSocket>
#include "dtranms_proto.h"

class DtranMsInstance : public QObject
{
    Q_OBJECT

private:
    bool checkPacket(const void *data, uint size);
    void onReceive(const QByteArray &data);
    void onNlpPacket(const void *data, int size);
    void onClpPacket(const void *data, int size);
    void onDlpPacket(const void *data, int size);
    void onDlpResult(const DTRANMS::sDlpResult *result);
    void onClpState(DTRANMS::sClpState *state);

    bool sendNlpPacket(DTRANMS::ePID pid, const void *data, uint size);
    bool sendPacket(const void *data, uint size);

    bool sendClpPacket(DTRANMS::eCLP type, uint8_t SID, uint8_t CN, const void *data, uint size);

public:
    DtranMsInstance(QObject *parent, uint16_t port_src, QHostAddress host, uint16_t port_dst);
    virtual ~DtranMsInstance();

    /**
     * @brief sendDlpPacket - отправка пакета данных
     * @param target - идентификатор получателя данных
     * @param group - признак того, что идентификатор это группа
     * @param FEC - способ помехоустойчивого кодирования
     * @param SID - идентификатор службы получателя
     * @param DN - порядковый номер пакета
     * @param data
     * @param size
     * @return признак успешной записи пакета в сокет
     */
    bool sendDlpPacket(DMRID target,
                       bool group,
                       DTRANMS::eFEC FEC,
                       uint8_t SID,
                       uint8_t DN,
                       const void *data,
                       uint32_t size);

    /**
     * @brief getGatewayState - получение состояни радиостанции или шлюза, может использоваться для проверки соединения.
     * обработка ответа в функции onClpState
     * @return признак успешной записи пакета в сокет
     */
    bool getGatewayState();

    static const char *b2s(const void *data, uint size);
public slots:
    void readFromPort();
signals:
    void receiveDLP(DMRID source, uint8_t SID, uint8_t DN, QByteArray data);
    void sendComplete(uint8_t DN);

protected:
    QUdpSocket * m_socket;
    QHostAddress m_host;
    uint16_t m_port_dst;

    int m_bsnNlp = -1;
    uint8_t m_fsnNlp = 0;
};

#endif // DTRANMSINSTANCE_H
