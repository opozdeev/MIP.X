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
#define READ_DISCRETE_INPUTS        0x02
#define READ_HOLDING_REGISTERS      0x03
#define READ_INPUT_REGISTERS        0x04
#define WRITE_SINGLE_COIL           0x05
#define WRITE_SINGLE_REGISTER       0x06
#define READ_EXCEPTION_STATUS       0x07
#define READ_WRITE_MULTIPLE_REGISTERS    0x17


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

#define QUANTITY_TO_WRITE_REGISTERS_HI  8
#define QUANTITY_TO_WRITE_REGISTERS_LO  9

#define WRITE_BYTE_COUNT            10

#define U_CALIBR_VALUE_HI           11
#define U_CALIBR_VALUE_LO           12

#define I_CALIBR_VALUE_HI           13
#define I_CALIBR_VALUE_LO           14

#define U_CALIBR_VALUE_SEND_HI      3
#define U_CALIBR_VALUE_SEND_LO      4
#define I_CALIBR_VALUE_SEND_HI      5
#define I_CALIBR_VALUE_SEND_LO      6

#define COIL_VALUE_HI               4
#define COIL_VALUE_LO               5

#define BYTE_COUNT                  2
#define COILS_STATUS                3

#define ERROR_CODE                  1
#define EXCEPTION_CODE              2

#define OFF_500V_COIL               0
#define ON_500V_PLUS_COIL           1
#define ON_500V_MINUS_COIL          2
#define ON_GROUND_COIL              3

void send_Coils(uint8_t receive[EUSART1_RX_BUFFER_SIZE]);
void send_Input_Registers(uint8_t receive[EUSART1_RX_BUFFER_SIZE]);
void send_Write_Coil(uint8_t receive[EUSART1_RX_BUFFER_SIZE]);
void write_Calibr_Coefs(uint8_t receive[EUSART1_RX_BUFFER_SIZE]);

void send_Error_Code(uint8_t receive[EUSART1_RX_BUFFER_SIZE], uint8_t exception);
uint8_t send_shot_ir_answer(uint8_t *request);

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

