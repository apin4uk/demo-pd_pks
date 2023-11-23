#ifndef DTRANMS_PROTO_H
#define DTRANMS_PROTO_H
#include <stdint.h>

#pragma pack(push, 1)

typedef uint32_t DMRID;

namespace DTRANMS
{
enum eGlobal
{
    NLP_VERSION        = 1,
    TIMEOUT_INIT       = 1000,  // ������ ������� �������������
    TIMEOUT_INIT_REPLY = 10,    // ����� �������� ������� �������������
    MTU                = 1500,  // ������������ ����� �������� ������
};
enum ePID
{
    pidCLP = 1,
    pidDLP = 2
};
enum eSID
{
    SID_KLUB            = 1,
    SID_TRANSPARENT     = 2,  // ���������� ������ ��� ����������������� �����
    SID_RDPS            = 3,
    SID_BRUS_MK         = 4,
    SID_ISAVP_RT        = 5,    // �����-�� ������� ����������� ������.
    SID_TRANSPARENT_MIN = 240,  // ���������� ������ ���
    SID_TRANSPARENT_MAX = 250,  // IP ��� CAN �����������
    SID_FIRMWARE        = 253,  // ������ ���������� �� �� �� �������
    SID_CPS             = 254,  // ������ ��������� �� �� �������
};
enum eSFN
{
    sfnDmrPrivate       = 0,
    sfnDmrGroup         = 1,
    sfnIPv4             = 2,
    sfnFunctionalNumber = 3,
    sfnSocketId         = 4,
    sfnPSTN             = 5,
};
enum ePRIOR
{
    priorNormal,
    priorHigh,
    priorEmergency,
};

enum eSTATE
{
    stateNone,
    stateNormal,
    stateAlarm,
};
enum eChType
{
    chtypeNone,
    chtypeGMV,
    chtypeDMR,

};
enum eChState
{
    chstateNormal,
    chstateNoLink,
    chstateAlarmAFU,
    chstateAlarmBoard,

};
enum eChResult
{
    chresPacketTransmit      = 0x00,
    chresPacketInQueue       = 0x01,
    chresPacketRemoveTimeout = 0x02,
    chresPacketRemoveReplace = 0x03,
    chresQueueFree           = 0x08,
    //chresChFree=0x04,

    chresError         = 0x80,
    chresChBusy        = 0x81,
    chresChMissing     = 0x82,
    chresChAlarm       = 0x83,
    chresQueueOverload = 0x88,
};

enum eCLP : uint8_t
{
    ///���������
    INIT         = 0x00,  ///<�������������
    Q_SUBSCRIBE  = 0x01,  ///<�������� �� ��������������
    A_SUBSCRIBE  = 0x81,  ///<������������� ��������
    Q_STATE      = 0x02,  ///<������ ���������� � ��������� �� � ��������� ������������
    A_STATE      = 0x82,  ///<���������� � ��������� � ��������� ������������
    Q_TIME       = 0x03,  ///<������ �������
    A_TIME       = 0x83,  ///<����� �� ������ �������
    Q_LOCATION   = 0x04,  ///<������ ������������� ������
    A_LOCATION   = 0x84,  ///<������ �������������� ���������
    Q_CH_STATE   = 0x10,  ///<������ ���������� � �����������
    A_CH_DMR     = 0x90,  ///<��������� ����������� DMR
    A_CH_GSM     = 0x91,  ///<��������� ����������� GSM-R
    A_CH_THURAYA = 0x92,  ///<��������� ����������� Thuraya

    ///���������� ������� �������� ������
    C_XON        = 0x20,  ///<���������� ��������
    C_XOFF       = 0x21,  ///<������ �������� (����� ����� ��� ����������)
    C_RESULT_DLP = 0x22,  ///<��������� �������� ������
    C_RESULT_CLP = 0x23,  ///<��������� ��������� ������ CLP
    C_CON        = 0x24,  ///<��������� ����������
    C_DISCON     = 0x25,  ///<������ ����������

    ///���������� �� ��������
    I_CH_DMR     = 0x40,  ///<��������� ����������� DMR
    I_CH_GSM     = 0x41,  ///<��������� ����������� GSM-R
    I_CH_THURAYA = 0x42,  ///<��������� ����������� Thuraya
    I_STATE      = 0x50,  ///<���������� � ��������� � ��������� ������������
    I_LOCATION   = 0x51,  ///<������ �������������� ���������
    I_TIME       = 0x52,  ///<������� �����
};
enum eVID
{
    vidPulsar,
    vidIRZ,
};
enum eFID
{
    fidStandard,
    fidPulsar,
    fidIrz,
};
enum eModelPulsar
{
    modelRLSM10_64,
    modelRMU4,
};
enum eFEC {
    fec1 = 0,
    fec3_4,
    fec1_2,
};
enum eTM { tmUndef = 0, tm2s = 1, tm4s = 2, tm8s = 3 };

enum eDmrState
{
    dmrNormal,
    dmrHandover,
    dmrScanning,
    dmrError        = 0x80,
    dmrNoSynch      = 0x81,
    dmrNoColourCode = 0x82,
};
struct sUdpHeader {
    uint16_t crc;
    uint16_t reserv;
};
typedef struct {
    uint16_t crc;
    uint16_t len;
    uint16_t reserv;
} sSerialHeader;
const uint8_t serialMaker[2] = {0xAA, 0x55};

struct sNlpHeader {
    uint8_t PID : 4;
    uint8_t VER : 4;
    uint8_t HL : 4;
    uint8_t reserv : 4;

    uint8_t len_hi : 4;
    uint8_t FSN : 4;
    uint8_t len_lo : 8;
    void setLen(uint16_t len)
    {
        len_hi = len >> 8;
        len_lo = len & 0xFF;
    }
    uint16_t getLen() { return (len_hi << 8) | len_lo; }
};

struct sClpHeader {
    uint8_t type;
    uint8_t CN;
    uint8_t FID;
    uint8_t SID;
};

struct sDlpHeader {
    uint8_t SID;
    uint8_t DN;

    union sDlpFlags
    {
        struct {
            uint8_t TM : 2;
            uint8_t SFN : 3;
            uint8_t CH : 3;
            uint8_t FEC : 2;
            uint8_t PRIOR : 2;
            uint8_t reserv : 4;
        };
        uint16_t d;
    } flags;
    uint8_t address[8];

    DMRID dmrId() { return (address[0] << 16) | (address[1] << 8) | (address[2]); }
    void setDmrId(DMRID dmrID)
    {
        address[0] = (dmrID >> 16) & 0xFF;
        address[1] = (dmrID >> 8) & 0xFF;
        address[2] = (dmrID & 0xFF);
    }
};

struct sClpInit
{
    uint8_t reserv : 4;
    uint8_t VID : 4;
};

struct sClpQState
{
    uint8_t reserv : 5;
    uint8_t CH : 3;
};

struct sClpState {
    uint8_t reserv : 4;
    uint8_t VID : 4;
    uint8_t MODEL;
    uint32_t SN;
    uint8_t MF_YEAR;
    uint8_t MF_MON;
    uint8_t VER_HARD;
    uint8_t VER_MAJOR;
    uint8_t VER_MINOR;
    uint32_t SOFT_DATE;
    uint16_t SOFT_CRC;

    uint8_t STATE_SM_RESERV : 7;
    uint8_t STATE_SM : 1;

    uint8_t STATE_PU2 : 2;
    uint8_t STATE_PU1 : 2;
    uint8_t STATE_MP2 : 2;
    uint8_t STATE_MP1 : 2;

    uint8_t STATE_RESERV : 4;
    uint8_t STATE_REG : 2;
    uint8_t STATE_GPS : 2;
    uint16_t reserv1;

    struct ChState {
        uint8_t state : 4;
        uint8_t type : 4;
    } CH_STATE[5];
};
struct sClpDmrState {
    uint8_t reserv : 5;
    uint8_t CH : 3;
    uint32_t VERSION;
    uint8_t DMR_STATE;
    uint16_t DMR_POWER;
    int8_t RSSI;
    uint8_t SNR;
    uint16_t SWR;
    uint32_t R_FREQ;
    uint32_t T_FREQ;
    uint8_t DMR_MODE;

    uint16_t reserv2 : 6;
    uint16_t DMR_RATE : 10;
    uint16_t BSID;
};
struct sClpXon {
    uint8_t reserv : 5;
    uint8_t ch : 3;
};
struct sClpXoff {
    uint8_t reserv : 5;
    uint8_t ch : 3;
    uint8_t timeout;
};
struct sDlpResult {
    uint8_t reserv : 5;
    uint8_t ch : 3;
    uint8_t DN;
    uint8_t Result;
};
struct sQueryState {
    uint8_t reserv : 5;
    uint8_t ch : 3;
};
}  // namespace SAUT
#pragma pack(pop)
#endif  // DTRANMS_PROTO_H
