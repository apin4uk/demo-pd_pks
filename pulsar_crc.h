#ifndef PULSAR_CRC_H
#define PULSAR_CRC_H

#include <stdint.h>

/**
 * @brief crc16 - вычисление CRC16 с полиномом 1021 (CRC-16-CCITT)
 * @param data - данные
 * @param size - размер данных
 * @param mask - маска накладываемая на результат
 * @param initial - начальное значение аккумулятора
 * @return
 */
uint16_t crc16(const void* data, uint32_t size, uint16_t mask, uint16_t initial);

/**
 * @brief crc32 - вычисление CRC32 с полиномом EBB88320 (CRC-32-IEEE 802.3)
 * @param data - данные
 * @param size - размер данных
 * @param mask - маска накладываемая на результат
 * @param initial - начальное значение аккумулятора
 * @return
 */
uint32_t crc32(const void* data, uint32_t size, uint32_t mask, uint32_t initial);


#endif  // PULSAR_CRC_H
