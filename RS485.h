#ifndef RS485_H
#define	RS485_H

#include "mcc_generated_files/mcc.h"
#include "high_voltage_control.h"
#include "measure.h"
#include "address.h"
#include "frame_types.h"
#include "CRC16.h"
#include "mcc_generated_files/memory.h"

extern volatile uint8_t eusart1TxHead;
extern volatile uint8_t eusart1TxTail;
//extern volatile uint8_t *eusart1TxBuffer;
extern volatile uint8_t eusart1TxBufferRemaining;

void save_measure(measures measure);
void send_answer(void);
void recieve_frame(uint8_t size);

void send(uint8_t *chptr, uint8_t size);
void send_done(void);
#endif	/* RS485_H */

