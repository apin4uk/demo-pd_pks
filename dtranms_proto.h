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
    TIMEOUT_INIT       = 1000,  // период пакетов инициализации
    TIMEOUT_INIT_REPLY = 10,    // число повторов пакетов инициализации
    MTU                = 1500,  // максимальная длина полезных данных
};
enum ePID
{
    pidCLP = 1,
    pidDLP = 2
};
enum eSID
{
    SID_KLUB            = 1,
    SID_TRANSPARENT     = 2,  // прозрачные данные для последовательного порта
    SID_RDPS            = 3,
    SID_BRUS_MK         = 4,
    SID_ISAVP_RT        = 5,    // ИСАВП-РТ система виртуальной сцепки.
    SID_TRANSPARENT_MIN = 240,  // прозрачные данные для
    SID_TRANSPARENT_MAX = 250,  // IP или CAN интерфейсов
    SID_FIRMWARE        = 253,  // данные обновления ПО РС по воздуху
    SID_CPS             = 254,  // данные настройки РС по воздуху
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
    ///Состояние
    INIT         = 0x00,  ///<Инициализация
    Q_SUBSCRIBE  = 0x01,  ///<Подписка на информирование
    A_SUBSCRIBE  = 0x81,  ///<Подтверждение подписки
    Q_STATE      = 0x02,  ///<Запрос информации о состоянии РС и доступных радиоканалах
    A_STATE      = 0x82,  ///<Информация о состоянии и доступных радиоканалах
    Q_TIME       = 0x03,  ///<Запрос времени
    A_TIME       = 0x83,  ///<Ответ на запрос времени
    Q_LOCATION   = 0x04,  ///<Запрос навигационных данных
    A_LOCATION   = 0x84,  ///<Данные навигационного приемника
    Q_CH_STATE   = 0x10,  ///<Запрос информации о радиоканале
    A_CH_DMR     = 0x90,  ///<Состояние радиоканала DMR
    A_CH_GSM     = 0x91,  ///<Состояние радиоканала GSM-R
    A_CH_THURAYA = 0x92,  ///<Состояние радиоканала Thuraya

    ///Управление каналом передачи данных
    C_XON        = 0x20,  ///<Разрешение передачи
    C_XOFF       = 0x21,  ///<Запрет передачи (канал занят или неисправен)
    C_RESULT_DLP = 0x22,  ///<Результат отправки пакета
    C_RESULT_CLP = 0x23,  ///<Результат обработки пакета CLP
    C_CON        = 0x24,  ///<Установка соединения
    C_DISCON     = 0x25,  ///<Разрыв соединения

    ///Информация по подписке
    I_CH_DMR     = 0x40,  ///<Состояние радиоканала DMR
    I_CH_GSM     = 0x41,  ///<Состояние радиоканала GSM-R
    I_CH_THURAYA = 0x42,  ///<Состояние радиоканала Thuraya
    I_STATE      = 0x50,  ///<Информация о состоянии и доступных радиоканалах
    I_LOCATION   = 0x51,  ///<Данные навигационного приемника
    I_TIME       = 0x52,  ///<Текущее время
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
