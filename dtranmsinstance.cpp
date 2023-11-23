#include "dtranmsinstance.h"
#include <QtEndian>
#include "dtranms_proto.h"
#include "pulsar_crc.h"

void log(const char *format, ...);
void info(const char *format, ...);

DtranMsInstance::DtranMsInstance(QObject *parent,
                                 uint16_t port_src,
                                 QHostAddress host,
                                 uint16_t port_dst)
    : QObject(parent)
    , m_host(host)
    , m_port_dst(port_dst)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readFromPort()));

    if (!m_socket->bind(port_src)) {
        throw std::runtime_error("UDP port open error");
    }
}
DtranMsInstance::~DtranMsInstance()
{
    if (m_socket)
        m_socket->close();
}

void DtranMsInstance::readFromPort()
{
    while (m_socket && m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        if (datagram.size()) {
            m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            onReceive(datagram);
        } else
            break;
    }
}
const char *DtranMsInstance::b2s(const void *data, uint size)
{
    static char szTemp[1024];
    uint index = 0;
    for (uint i = 0; (i < size) && ((index + 3) < sizeof(szTemp)); i++) {
        index += snprintf(szTemp + index, sizeof(szTemp) - index, "%02X ", ((uchar *) data)[i]);
        if (i >= (sizeof(szTemp) - 4)) {
            index += sprintf(szTemp + index, "...");
            break;
        }
    }
    if (index == (sizeof(szTemp) - 1))
        szTemp[index - 1] = 0;
    return szTemp;
}
bool DtranMsInstance::checkPacket(const void *data, uint size)
{
    uint16_t rcrc;
    memcpy(&rcrc, data, sizeof(rcrc));
    uint16_t crc = crc16((char *) data + 2, size - 2, 0, 0xFFFF);

    if (crc != qFromBigEndian(rcrc)) {
        info("ERROR CRC_HDR[len:%d]:%s", size, b2s(data, size));
        return false;
    }

    return true;
}
void DtranMsInstance::onReceive(QByteArray const &data)
{
    if (!checkPacket(data.data(), data.size()))
        return;

    onNlpPacket((DTRANMS::sUdpHeader *) data.data() + 1, data.size() - sizeof(DTRANMS::sUdpHeader));
}
void DtranMsInstance::onNlpPacket(const void *data, int size)
{
    DTRANMS::sNlpHeader *hdr = (DTRANMS::sNlpHeader *) data;

    if (size < sizeof(DTRANMS::sNlpHeader)) {
        info("ERROR LEN:size smaller than header");
        return;
    }

    if (hdr->getLen() != (size - sizeof(*hdr))) {
        info("ERROR LEN:wait=%u received=%u", hdr->getLen(), (size - sizeof(*hdr)));
        return;
    }

    if ((m_bsnNlp != -1) && (hdr->FSN != ((m_bsnNlp + 1) & 0xF))) {
        info("ERROR FSN:wait=%u received=%u", (m_bsnNlp + 1) & 0x0F, hdr->FSN);
        return;
    }

    m_bsnNlp = hdr->FSN;

    switch (hdr->PID) {
    case DTRANMS::pidCLP:
        onClpPacket(hdr + 1, size - sizeof(*hdr));
        break;
    case DTRANMS::pidDLP:
        onDlpPacket(hdr + 1, size - sizeof(*hdr));
        break;
    default:
        info("ERROR: Receive unknown packet [len:%3d]:%s", size, b2s(data, size));
    }
}
void DtranMsInstance::onClpPacket(const void *data, int size)
{
    DTRANMS::sClpHeader *hdr = (DTRANMS::sClpHeader *) data;

    log("recv CLP [len:%3d]: SID=%u %s", size, hdr->SID, b2s(data, size));

    switch (hdr->type) {
    case DTRANMS::C_RESULT_DLP:
        onDlpResult((DTRANMS::sDlpResult *) (hdr + 1));
        break;
    case DTRANMS::A_STATE:
        onClpState((DTRANMS::sClpState *) (hdr + 1));
        break;
    }
}
void DtranMsInstance::onDlpResult(const DTRANMS::sDlpResult *result)
{
    switch (result->Result) {
    case DTRANMS::chresPacketTransmit:
        //info("Packet %d - sended", result->DN);
        emit sendComplete(result->DN);
        break;
    case DTRANMS::chresPacketInQueue:
        //info("Packet %d - in queue", result->DN);
        break;
    }
}
void DtranMsInstance::onDlpPacket(const void *data, int size)
{
    DTRANMS::sDlpHeader *hdr = (DTRANMS::sDlpHeader *) data;

    if (hdr->flags.SFN == DTRANMS::sfnDmrPrivate)
        log("recv DLP [len:%3d]: SRC=%u SID=%d %s", size, hdr->SID, hdr->dmrId(), b2s(data, size));
    else {
        info("ERROR: source DMRID is not private");
        return;
    }

    emit receiveDLP(hdr->dmrId(),
                    hdr->SID,
                    hdr->DN,
                    QByteArray((char *) (hdr + 1), size - sizeof(*hdr)));
}

bool DtranMsInstance::sendDlpPacket(DMRID target,
                                    bool group,
                                    DTRANMS::eFEC FEC,
                                    uint8_t SID,
                                    uint8_t DN,
                                    const void *data,
                                    uint32_t size)
{
    static char temp[1500 + sizeof(DTRANMS::sDlpHeader)];
    DTRANMS::sDlpHeader *hdr = (DTRANMS::sDlpHeader *) temp;
    memset(hdr, 0, sizeof(*hdr));

    hdr->SID = SID;
    hdr->flags.TM = DTRANMS::tm2s;
    hdr->flags.FEC = FEC;
    hdr->flags.PRIOR = DTRANMS::priorNormal;

    hdr->DN = DN;

    hdr->flags.SFN = group ? DTRANMS::sfnDmrGroup : DTRANMS::sfnDmrPrivate;
    hdr->setDmrId(target);
    memcpy(hdr + 1, data, size);

    return sendNlpPacket(DTRANMS::pidDLP, hdr, size + sizeof(*hdr));
}

bool DtranMsInstance::sendNlpPacket(DTRANMS::ePID pid, const void *data, uint size)
{
    static uchar temp[1500 + sizeof(DTRANMS::sNlpHeader)];

    DTRANMS::sNlpHeader *hdr = (DTRANMS::sNlpHeader *) temp;
    memset(hdr, 0, sizeof(*hdr));

    if ((size + sizeof(*hdr)) > sizeof(temp)) {
        return false;
    }

    hdr->VER = DTRANMS::NLP_VERSION;
    hdr->PID = pid;
    hdr->HL = 0;
    hdr->FSN = m_fsnNlp++;
    hdr->setLen(size);
    memcpy(hdr + 1, data, size);

    return sendPacket(hdr, sizeof(*hdr) + size);
}
bool DtranMsInstance::sendPacket(const void *data, uint size)
{
    static char temp[1500 + sizeof(DTRANMS::sNlpHeader) + sizeof(DTRANMS::sUdpHeader)];
    DTRANMS::sUdpHeader *hdr = (DTRANMS::sUdpHeader *) temp;
    memset(hdr, 0, sizeof(*hdr));

    if ((size + sizeof(*hdr)) > sizeof(temp)) {
        return false;
    }
    memcpy(hdr + 1, data, size);
    uint16_t crc = crc16(temp + 2, size + sizeof(*hdr) - sizeof(crc), 0, 0xFFFF);
    hdr->crc = qToBigEndian(crc);

    if (m_socket->isValid()) {
        quint64 res = m_socket->writeDatagram(temp, size + sizeof(*hdr), m_host, m_port_dst);
        if ((size_t) res == (size + sizeof(*hdr))) {
            log("send[len:%3d]:%s", size + sizeof(*hdr), b2s(temp, size + sizeof(*hdr)));
            return true;
        } else
            info("ERROR: Send packet - %s", qPrintable(m_socket->errorString()));
    }

    return false;
}
bool DtranMsInstance::sendClpPacket(
    DTRANMS::eCLP type, uint8_t SID, uint8_t CN, const void *data, uint size)
{
    static char temp[256];
    DTRANMS::sClpHeader *hdr = (DTRANMS::sClpHeader *) temp;
    memset(hdr, 0, sizeof(*hdr));

    hdr->type = type;
    hdr->CN = CN;
    hdr->FID = 0;
    hdr->SID = SID;
    memcpy(hdr + 1, data, size);

    log("send CLP:Type=%d CN=%d FID=%d SID=%d - %s",
        hdr->type,
        hdr->CN,
        hdr->FID,
        hdr->SID,
        b2s(hdr, size + sizeof(*hdr)));

    return sendNlpPacket(DTRANMS::pidCLP, hdr, sizeof(*hdr) + size);
}
bool DtranMsInstance::getGatewayState()
{
    DTRANMS::sClpQState pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.CH = 0;
    return sendClpPacket(DTRANMS::Q_STATE, 0, 0, &pkt, sizeof(pkt));
}
void DtranMsInstance::onClpState(DTRANMS::sClpState *state)
{
    info("Receive state from gateway:\n"
         "  vendor:%u\n"
         "  model:%u\n"
         "  SN:%u\n"
         "  MF_DATE:2%03u.%02u\n"
         "  VERSION:%u.%u h%u",
         state->VID,
         state->MODEL,
         qFromBigEndian(state->SN),
         state->MF_YEAR,
         state->MF_MON + 1,
         state->VER_MAJOR,
         state->VER_MINOR,
         state->VER_HARD);
}
