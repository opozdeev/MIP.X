#include "RS485.h"



//answer_frame answer;

/* Адреса COILов
 * 
 * 1 ВЫКЛ 500В
 * 2 ВКЛ +500В
 * 3 ВКЛ -500В
 * 4 ЗЕМЛЯ
 */

/* Адреса Input Registers полного запроса данных
 *  (каждое измерение представлено float и упаковывано в 2 регистра Modbus)
 * Регистры     - Описание
 * 2000,2001    - Сопротивление изоляции полюса относительно земли, МОм
 * 2002,2003    - Напряжение ИОНа, В
 * 2004,2005    - Ток утечки через изоляцию, мкА
 * 2006,2007    - Напряжение полюса, В
 */

/* Адреса Input Registers для краткого запроса
 *  (всего два измерения: напряжение и споротивление полюса, каждое в одном регистре Modbus )
 * Регистр     - Описание
 * 1 - Напряжение полюса, увеличинное в 100 раз, В. 
 *      Значению регистра 12345, соответствует напряжение 123,45 В.
 * 2 - Сопротивление изоляции полюса, кОм.
 *      Диапазон: 1-65000,кОм
 */

/* Адреса регистров для чтения/записи данных устройства
 * Регистр     - Описание
 * 1000 - номер серии/версия харда (печатной платы/принципиальной схемы)
 * 1001 - порядковый номер в серии (старшие два байта)
 * 1002 - порядковый номер в серии (младшие два байта)
 * 1003 - версия прошивки (в регистре 12 -> версия 1.2)
 * 1004 - дата производства/первой калибровки  (101220 -> 10/12/2021)
 * 1005 - дата последней калибровки (141021 -> 14/10/21)
 * 1006 - общее количесво пройденых калибровок
 *  
 * Адреса в eeprom для хранения данных устройства
 * Адрес eeprom <- Регистр
 * 0x100,0x101  <- 1000
 * 0x102,0x103  <- 1001
 * 0x104,0x105  <- 1002
 * 0x106,0x107  <- 1003
 * 0x108,0x109  <- 1004
 * 0x10a,0x10b  <- 1005
 * 0x10c,0x10d  <- 1006  
 */

/* Адреса Регистров коэффициентов 
 * 
 * 20 R - Resistance
 * 21 I - Current HV
 * 22 U - Voltage HV
 * 23 Uin - Voltage LV
 */

extern bool OFF_500V;
extern bool ON_500V_Plus;
extern bool ON_500V_Minus;
extern bool Ground;

static measures response_measure;
static bool TXState = false;//состояние передачи

static uint8_t send_short_ir_answer(uint8_t *request);
static uint8_t send_long_ir_answer(uint8_t *request);
static uint8_t send_device_ir_info(uint8_t *request);

// | сюда после срабатывания таймера, то есть после свободности в линии после принятия кадра
// V размер принятого кадра меньше размера буфера приемника УАРТ
void recieve_frame(uint8_t size)
{
    uint8_t request[EUSART1_RX_BUFFER_SIZE], i;
    
    for (i = 0; i < size; i++)
    {
        request[i] = EUSART1_Read();
    }
    
    uint16_t receive_CRC = 0;
    
    receive_CRC = request[size-1];
    receive_CRC <<= 8;
    receive_CRC |= request[size-2];
    
    if (receive_CRC != CRC16(request, size-2))
        return;
    
    if (request[ADDRESS] != get_addr())
        return;
    
    switch (request[FUNCTION])
    {
        case READ_COILS:
        {
            send_Coils(request);
            break;
        }
        case READ_DISCRETE_INPUTS:
        {
            //ФУНКЦИЯ НЕ ПОДДЕРЖИВАЕТСЯ EXCEPTION_CODE == 1
            send_Error_Code(request, 1);
            break;
        }
        case READ_HOLDING_REGISTERS:
        {
            //ФУНКЦИЯ НЕ ПОДДЕРЖИВАЕТСЯ EXCEPTION_CODE == 1
            send_Error_Code(request, 1);
            break;
        }
        case READ_INPUT_REGISTERS:
        {
            send_Input_Registers(request);
            break;
        }
        case WRITE_SINGLE_COIL:
        {
            send_Write_Coil(request);
            break;
        }
        case READ_EXCEPTION_STATUS:
        {
            //ФУНКЦИЯ НЕ ПОДДЕРЖИВАЕТСЯ EXCEPTION_CODE == 1
            send_Error_Code(request, 1);
            break;
        }
        case READ_WRITE_MULTIPLE_REGISTERS:
        {
            //send_Error_Code(request, 1);
            write_Calibr_Coefs(request);
            break;
        }
        default:
        {
            //ФУНКЦИЯ НЕ ПОДДЕРЖИВАЕТСЯ EXCEPTION_CODE == 1
            send_Error_Code(request, 1);
            break;
        }
    }
}

void send_Coils(uint8_t *request)
{
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] > 3 ||
        request[QUANTITY_OF_COILS_LO] < 1 ||
        request[QUANTITY_OF_COILS_LO] > 4 ||
        request[QUANTITY_OF_COILS_LO] > (4-request[STARTING_ADDRESS_LO]) ||     
        request[STARTING_ADDRESS_HI] != 0 ||
        request[QUANTITY_OF_COILS_HI] != 0)
    {
        //EXCEPTION_CODE == 2
        send_Error_Code(request, 2);
        return;
    }
        
    uint8_t response[6];
    
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_COILS;
    response[BYTE_COUNT] = 0x01;
    response[COILS_STATUS] = 0x00 | (Ground<<3) | (ON_500V_Minus<<2) | (ON_500V_Plus<<1) | (OFF_500V<<0);
    response[COILS_STATUS] >>= request[STARTING_ADDRESS_LO];
    response[COILS_STATUS] <<= (8 - request[QUANTITY_OF_COILS_LO]);
    response[COILS_STATUS] >>= (8 - request[QUANTITY_OF_COILS_LO]);
    
    //рассчитать СRC
    uint16_t tempCRC;
    
    tempCRC = CRC16(response, sizeof(response)-2);
    
    response[4] = (uint8_t)(tempCRC);
    response[5] = (uint8_t)(tempCRC >> 8);
    
    send(response, sizeof(response));
}

void send_Input_Registers(uint8_t *request)
{
    if ( 1 == send_short_ir_answer( request ) )
        return;
    if ( 1 == send_long_ir_answer(request) )
        return;
    if ( 1 == send_device_ir_info(request) )
        return;
    
    send_Error_Code(request, 2); //EXCEPTION_CODE == 2
}

static uint8_t send_device_ir_info(uint8_t *request)
{
    if (request[STARTING_ADDRESS_LO] < 0xE8 ||
        request[STARTING_ADDRESS_LO] > 0xEE ||
        request[STARTING_ADDRESS_HI] != 0x03 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 7 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        return 0;
    }
    
    union REG
    {
        uint16_t w;
        uint8_t b[2];
    };
    union REG hard_ver, serial_number_hight, serial_number_low, soft_ver;
    eeprom_read_object( 0x100, &hard_ver.w, sizeof(uint16_t) );
    eeprom_read_object( 0x102, &serial_number_hight.w, sizeof(uint16_t) );
    eeprom_read_object( 0x104, &serial_number_low.w, sizeof(uint16_t) );
    eeprom_read_object( 0x106, &soft_ver.w, sizeof(uint16_t) );

    union REG create_date, last_calibrate_date, number_of_calibrate;
    eeprom_read_object( 0x108, &create_date.w, sizeof(uint16_t) );
    eeprom_read_object( 0x10a, &last_calibrate_date.w, sizeof(uint16_t) );
    eeprom_read_object( 0x10c, &number_of_calibrate.w, sizeof(uint16_t) );
    
    uint8_t response[40], i, j;
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = hard_ver.b[0];
    response[4] = hard_ver.b[1];
    response[5] = serial_number_hight.b[0];
    response[6] = serial_number_hight.b[1];
    response[7] = serial_number_low.b[0];
    response[8] = serial_number_low.b[1];
    response[9] = soft_ver.b[0];
    response[10] = soft_ver.b[1];
    response[11] = create_date.b[0];
    response[12] = create_date.b[1];
    response[13] = last_calibrate_date.b[0];
    response[14] = last_calibrate_date.b[1];
    response[15] = number_of_calibrate.b[0];
    response[16] = number_of_calibrate.b[1];
    
    request[STARTING_ADDRESS_HI] = request[STARTING_ADDRESS_HI] - 0x03;
    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0xE8;
    
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }
    
    uint16_t tempCRC;
    tempCRC = CRC16(response, j+1);
    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);

    send(response, j+3);
    return 1;
}
static uint8_t send_short_ir_answer(uint8_t *request)
{
    if (request[STARTING_ADDRESS_LO] < 0x01 ||
        request[STARTING_ADDRESS_LO] > 0x02 ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 2 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0 )
    {
        return 0;
    }
        
    union REG
    {
        uint16_t w;
        uint8_t b[2];
    };

    union REG volt, resist;
    resist.w = 12345;//(uint16_t) abs(response_measure.resistance) * 1000;    //MOm -> KOm
    volt.w = 54321;//(uint16_t) abs(response_measure.voltagein) * 100;

    uint8_t response[20], i, j;
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = volt.b[0];
    response[4] = volt.b[1];
    response[5] = resist.b[0];
    response[6] = resist.b[1];

    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0x01;
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }
    //рассчитать СRC
    uint16_t tempCRC;

    tempCRC = CRC16(response, j+1);

    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);

    send(response, j+3);
    return 1;
}

static uint8_t send_long_ir_answer ( uint8_t *request )    {
    
    if (request[STARTING_ADDRESS_LO] < 0xD0 ||
        request[STARTING_ADDRESS_LO] > 0xD7 ||
        request[STARTING_ADDRESS_HI] != 0x07 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 8 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        return 0;
    }
    
    request[STARTING_ADDRESS_HI] = request[STARTING_ADDRESS_HI] - 0x07;
    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0xD0;
    
    uint8_t response[18+4], i, j;
    union FloatChar
    {
        float fl;
        uint8_t ch[4];
    };
    
    union FloatChar resistance, voltage, current, voltagein;
    
    resistance.fl = 1;//response_measure.resistance;
    voltage.fl = 2;//response_measure.voltage;
    current.fl = 3;//response_measure.current;
    voltagein.fl = 4;//response_measure.voltagein;
        
    
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = resistance.ch[0];
    response[4] = resistance.ch[1];
    response[5] = resistance.ch[2];
    response[6] = resistance.ch[3];
    response[7] = voltage.ch[0];
    response[8] = voltage.ch[1];
    response[9] = voltage.ch[2];
    response[10] = voltage.ch[3];
    response[11] = current.ch[0];
    response[12] = current.ch[1];
    response[13] = current.ch[2];
    response[14] = current.ch[3];
    
    response[15] = voltagein.ch[0];
    response[16] = voltagein.ch[1];
    response[17] = voltagein.ch[2];
    response[18] = voltagein.ch[3];
    
    
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }
    
    //3 + request[STARTING_ADDRESS_LO]*2; // откуда
    //3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]; // докуда
   
    //рассчитать СRC
    uint16_t tempCRC;
    tempCRC = CRC16(response, j+1);
    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);
    
    send(response, j+3);
    return 1;
}

void send_Write_Coil(uint8_t *request)
{
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] > 3 ||
        request[STARTING_ADDRESS_HI] != 0 ||
        request[COIL_VALUE_HI] != 0xFF ||
        request[COIL_VALUE_LO] != 0x00)
    {
        //EXCEPTION_CODE == 2
        send_Error_Code(request, 2);
        return;
    }
    switch(request[STARTING_ADDRESS_LO])
    {
        case OFF_500V_COIL:
        {
            TurnOFF_500V();
            break;
        }
        case ON_500V_PLUS_COIL:
        {
            TurnON_500V_Plus();
            break;
        }
        case ON_500V_MINUS_COIL:
        {
            TurnON_500V_Minus();
            break;
        }
        case ON_GROUND_COIL:
        {
            TurnON_GND();
            break;
        }
        default:
        {
            TurnOFF_500V();
            break;
        }
    }
    
    uint16_t tempCRC;
    
    tempCRC = CRC16(request, 8-2);
    
    request[6] = (uint8_t)(tempCRC);
    request[7] = (uint8_t)(tempCRC >> 8);    
    
    send(request, 8);
}

void write_Calibr_Coefs(uint8_t *request)
{
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] != 0xE8 ||
        request[STARTING_ADDRESS_HI] != 0x03 ||
        request[QUANTITY_OF_REGISTERS_LO] != 12 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0 ||
        request[WRITE_STARTING_ADDRESS_LO] != 0xE8 ||
        request[WRITE_STARTING_ADDRESS_HI] != 0x03 ||
        request[QUANTITY_TO_WRITE_REGISTERS_LO] != 12 ||
        request[QUANTITY_TO_WRITE_REGISTERS_HI] != 0 ||
        request[WRITE_BYTE_COUNT] != 24)
    {
        //EXCEPTION_CODE == 2
        send_Error_Code(request, 2);
        return;
    }
    
      
    union FloatChar
    {
        float fl;
        uint8_t ch[4];
    };
    union FloatChar U_coef, U_bias, I_above_100_coef, I_above_100_bias, I_below_100_coef, R_bias, Uin_coef, Uin_bias;
    
    U_coef.ch[0] = request[11];
    U_coef.ch[1] = request[12];
    U_coef.ch[2] = request[13];
    U_coef.ch[3] = request[14];
    U_bias.ch[0] = request[15];
    U_bias.ch[1] = request[16];
    U_bias.ch[2] = request[17];
    U_bias.ch[3] = request[18];
    I_above_100_coef.ch[0] = request[19];
    I_above_100_coef.ch[1] = request[20];
    I_above_100_coef.ch[2] = request[21];
    I_above_100_coef.ch[3] = request[22];
    I_above_100_bias.ch[0] = request[23];
    I_above_100_bias.ch[1] = request[24];
    I_above_100_bias.ch[2] = request[25];
    I_above_100_bias.ch[3] = request[26];
    I_below_100_coef.ch[0] = request[27];
    I_below_100_coef.ch[1] = request[28];
    I_below_100_coef.ch[2] = request[29];
    I_below_100_coef.ch[3] = request[30];
    R_bias.ch[0] = request[31];
    R_bias.ch[1] = request[32];
    R_bias.ch[2] = request[33];
    R_bias.ch[3] = request[34];
    /*
    Uin_coef.ch[0] = request[35];
    Uin_coef.ch[1] = request[36];
    Uin_coef.ch[2] = request[37];
    Uin_coef.ch[3] = request[38];
    Uin_bias.ch[0] = request[39];
    Uin_bias.ch[1] = request[40];
    Uin_bias.ch[2] = request[41];
    Uin_bias.ch[3] = request[42];
     */
    
    eeprom_write_object(0x01, &U_coef.fl, sizeof(float));
    eeprom_write_object(0x05, &U_bias.fl, sizeof(float));
    eeprom_write_object(0x09, &I_above_100_coef.fl, sizeof(float));
    eeprom_write_object(0x0D, &I_above_100_bias.fl, sizeof(float));
    eeprom_write_object(0x11, &I_below_100_coef.fl, sizeof(float));
    eeprom_write_object(0x15, &R_bias.fl, sizeof(float));
    eeprom_write_object(0x19, &Uin_coef.fl, sizeof(float));
    eeprom_write_object(0x1C, &Uin_bias.fl, sizeof(float));
    
    
    eeprom_read_object(0x01, &U_coef.fl, sizeof(float));
    eeprom_read_object(0x05, &U_bias.fl, sizeof(float));
    eeprom_read_object(0x09, &I_above_100_coef.fl, sizeof(float));
    eeprom_read_object(0x0D, &I_above_100_bias.fl, sizeof(float));
    eeprom_read_object(0x11, &I_below_100_coef.fl, sizeof(float));
    eeprom_read_object(0x15, &R_bias.fl, sizeof(float));
    eeprom_read_object(0x19, &Uin_coef.fl, sizeof(float));
    eeprom_read_object(0x1C, &Uin_bias.fl, sizeof(float));
    
    //uint8_t response[13];
    
    request[ADDRESS] = get_addr();
    request[FUNCTION] = READ_WRITE_MULTIPLE_REGISTERS;
    request[BYTE_COUNT] = 24;
    request[3] = U_coef.ch[0];
    request[4] = U_coef.ch[1];
    request[5] = U_coef.ch[2];
    request[6] = U_coef.ch[3];
    request[7] = U_bias.ch[0];
    request[8] = U_bias.ch[1];
    request[9] = U_bias.ch[2];
    request[10] = U_bias.ch[3];
    request[11] = I_above_100_coef.ch[0];
    request[12] = I_above_100_coef.ch[1];
    request[13] = I_above_100_coef.ch[2];
    request[14] = I_above_100_coef.ch[3];
    request[15] = I_above_100_bias.ch[0];
    request[16] = I_above_100_bias.ch[1];
    request[17] = I_above_100_bias.ch[2];
    request[18] = I_above_100_bias.ch[3];
    request[19] = I_below_100_coef.ch[0];
    request[20] = I_below_100_coef.ch[1];
    request[21] = I_below_100_coef.ch[2];
    request[22] = I_below_100_coef.ch[3];
    request[23] = R_bias.ch[0];
    request[24] = R_bias.ch[1];
    request[25] = R_bias.ch[2];
    request[26] = R_bias.ch[3];
    /*
    request[27] = Uin_coef.ch[0];
    request[28] = Uin_coef.ch[1];
    request[29] = Uin_coef.ch[2];
    request[30] = Uin_coef.ch[3];
    request[31] = Uin_bias.ch[0];
    request[32] = Uin_bias.ch[1];
    request[33] = Uin_bias.ch[2];
    request[34] = Uin_bias.ch[3];
     */
    
    //рассчитать СRC
    uint16_t tempCRC;
    
    tempCRC = CRC16(request, 27);
    
    request[27/*+8*/] = (uint8_t)(tempCRC);
    request[28/*+8*/] = (uint8_t)(tempCRC >> 8);
    
    send(request, 29/*+8*/);
}

void send_Error_Code(uint8_t *request, uint8_t exception)
{
    uint8_t response[5];
    
    response[ADDRESS] = get_addr();
    response[ERROR_CODE] = request[FUNCTION] + 0x80;
    response[EXCEPTION_CODE] = exception;
    
    //рассчитать CRC
    uint16_t tempCRC;
    
    tempCRC = CRC16(request, sizeof(response)-2);
    
    request[3] = (uint8_t)(tempCRC >> 8);
    request[4] = (uint8_t)(tempCRC);
    
    send(response, sizeof(response));
}
//отправка буфера chptr[] размером size
void send(uint8_t *chptr, uint8_t size)
{
    uint8_t i;
    
    TX_nRC_SetHigh();//переключаем на передачу
    for (i = 0; i < size; i++)
        EUSART1_Write(*chptr++);
    SetTXState();
}

void send_done(void)
{
    TX_nRC_SetLow();//переключаем на приём
}

void save_measure(measures measure)
{
    // Disable the Global Interrupts
    INTERRUPT_GlobalInterruptDisable();
    
    response_measure = measure;
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
}

void ResetTXState(void)
{
    TXState = false;
}

void SetTXState(void)
{
    TXState = true;
}

bool IsTXState(void)
{
    return TXState;
}