#ifndef PULSAR_CRC_H
#define PULSAR_CRC_H

#include <stdint.h>

/**
 * @brief crc16 - ���������� CRC16 � ��������� 1021 (CRC-16-CCITT)
 * @param data - ������
 * @param size - ������ ������
 * @param mask - ����� ������������� �� ���������
 * @param initial - ��������� �������� ������������
 * @return
 */
uint16_t crc16(const void* data, uint32_t size, uint16_t mask, uint16_t initial);

/**
 * @brief crc32 - ���������� CRC32 � ��������� EBB88320 (CRC-32-IEEE 802.3)
 * @param data - ������
 * @param size - ������ ������
 * @param mask - ����� ������������� �� ���������
 * @param initial - ��������� �������� ������������
 * @return
 */
uint32_t crc32(const void* data, uint32_t size, uint32_t mask, uint32_t initial);


#endif  // PULSAR_CRC_H
