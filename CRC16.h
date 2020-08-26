#ifndef CRC16_H
#define	CRC16_H

#include <stdint.h>
#include <xc.h>
#include "RS485.h"
#include "frame_types.h"

uint16_t CRC16(uint8_t *data, size_t len);
uint16_t CRC16table (uint8_t *nData, size_t wLength);

#endif	/* CRC16_H */

