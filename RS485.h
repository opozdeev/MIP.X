#ifndef RS485_H
#define	RS485_H

#include "mcc_generated_files/mcc.h"
#include "measure.h"


extern volatile uint8_t eusart1TxHead;
extern volatile uint8_t eusart1TxTail;
//extern volatile uint8_t *eusart1TxBuffer;
extern volatile uint8_t eusart1TxBufferRemaining;

void save_measure(measures measure);
void recieve_frame(uint8_t size);
void send_done(void);
void ResetTXState(void);
void SetTXState(void);
bool IsTXState(void);
#endif	/* RS485_H */

