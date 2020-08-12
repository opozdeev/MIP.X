#ifndef RS485_H
#define	RS485_H

#include "mcc_generated_files/mcc.h"
#include "high_voltage_control.h"
#include "measure.h"
#include "address.h"
#include "frame_types.h"
#include "CRC16.h"
#include "mcc_generated_files/memory.h"

#define READ_COILS                  0x01
#define READ_HOLDING_REGISTERS      0x03
#define READ_INPUT_REGISTERS        0x04
#define WRITE_SINGLE_COIL           0x05
#define WRITE_REGISERS              0x10


#define ERROR_WRONG_FUNCTION        0x01
#define ERROR_WRONG_REGISTER        0x02

#define ADDRESS                     0
#define FUNCTION                    1

#define STARTING_ADDRESS_HI         2
#define STARTING_ADDRESS_LO         3
#define QUANTITY_OF_COILS_HI        4
#define QUANTITY_OF_COILS_LO        5

#define QUANTITY_OF_REGISTERS_HI    4
#define QUANTITY_OF_REGISTERS_LO    5

#define WRITE_STARTING_ADDRESS_HI   6
#define WRITE_STARTING_ADDRESS_LO   7

#define COIL_VALUE_HI               4
#define COIL_VALUE_LO               5

#define BYTE_COUNT                  2
#define COILS_STATUS                3

#define OFF_500V_COIL               0
#define ON_500V_PLUS_COIL           1
#define ON_500V_MINUS_COIL          2
#define ON_GROUND_COIL              3

extern volatile uint8_t eusart1TxHead;
extern volatile uint8_t eusart1TxTail;
//extern volatile uint8_t *eusart1TxBuffer;
extern volatile uint8_t eusart1TxBufferRemaining;

void save_measure(measures measure);
void send_answer(void);
void recieve_frame(uint8_t size);

void send(uint8_t *chptr, uint8_t size);
void send_done(void);
void ResetTXState(void);
void SetTXState(void);
bool IsTXState(void);
#endif	/* RS485_H */

