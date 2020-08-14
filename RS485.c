#include "RS485.h"
#include "relay_control.h"

#include "high_voltage_control.h"
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
#define ON_RELE_COIL                4
//answer_frame answer;

/* Адреса COILов
 * 
 * 0 Откл
 * 1 Вкл +500В
 * 2 Вкл -500В
 * 3 Земля
 * 4 Релейный выход
 */

/* Адреса Input Registers для краткого запроса
 *  (всего два измерения: напряжение и споротивление полюса, каждое в одном регистре Modbus )
 * Регистр     - Описание
 * 1 - Напряжение полюса, увеличинное в 100 раз, В. 
 *      Значению регистра 12345, соответствует напряжение 123,45 В.
 * 2 - Сопротивление изоляции полюса, кОм.
 *      Диапазон: 1-65000,кОм
 */

/* Адреса Input Registers полного запроса данных
 *  (каждое измерение представлено float и упаковывано в 2 регистра Modbus)
 * Регистры          - Описание
 * 20,21 (0x14,0x15) - Напряжение полюса, В
 * 22,23 (0x16,0x17) - Сопротивление изоляции полюса относительно земли, МОм
 * 24,25 (0x18,0x19) - Ток утечки через изоляцию, мкА
 * 26,27 (0x1a,0x1b) - Напряжение ИОНа, В
 */

/* Адреса Input Registers регистров для чтения данных устройства
 * Регистр     - Описание
 * 100 (0x64) - номер серии/версия харда (печатной платы/принципиальной схемы)
 * 101 (0x65) - порядковый номер в серии (старшие два байта)
 * 102 (0x66) - порядковый номер в серии (младшие два байта)
 * 103 (0x67) - версия прошивки (в регистре 12 -> версия 1.2)
 * 104 (0x68) - месяц последней калибровки  (1219 -> декабрь 2019)
 * 105 (0x69) - месяц следущей калибровки (1223 -> декабрь 2023)
 * 106 (0x6a) - общее количесво пройденых калибровок
 *  
 * Адреса в eeprom для хранения данных устройства
 * Адрес eeprom <- Регистр
 * 0x100,0x101  <- 100
 * 0x102,0x103  <- 101
 * 0x104,0x105  <- 102
 * 0x106,0x107  <- 103
 * 0x108,0x109  <- 104
 * 0x10a,0x10b  <- 105
 * 0x10c,0x10d  <- 106  
 *  
 */
/*
 * Калибровочные коэффициенты храняться по следующи адресам в eepropm
 * Регистр   |      Описание                       |  Адреса eeprom
 * 01 (0x01) |  Коэффициент напряжения полюса:     |  0x00 - 0x03
 * 03 (0х03) |  Смещение напряжения полюса:        |  0x04 - 0x07
 * 05 (0х05) |  Коэффициент изоляции полюса:       |  0x08 - 0x0b
 * 07 (0х07) |  Смещение изоляции полюса:          |  0x0c - 0x0f
 * 09 (0х09) |  Коэффициент напряжения ИОН:        |  0x10 - 0x13
 * 11 (0х0b) |  Смещение напряжения ИОН:           |  0x14 - 0x17
 * 13 (0х0d) |  Коэффициент тока ИОН: (>100мкА):   |  0x18 - 0x1b
 * 15 (0х0f) |  Смещение тока ИОН:                 |  0x1c - 0x1f
 * 17 (0х11) |  Коэффициент тока ИОН (<100мкА):    |  0x20 - 0x23
 */

extern bool OFF_500V;
extern bool ON_500V_Plus;
extern bool ON_500V_Minus;
extern bool Ground;
extern bool relay_coil_state;
union FloatChar
{
        float fl;
        uint8_t ch[4];
};
union REG
{
        uint16_t w;
        uint8_t b[2];
};
static measures response_measure;
static bool TXState = false;//состояние передачи
static uint16_t tempCRC;
static uint8_t response[42]; 

static void read_input_registers(uint8_t* receive);
static uint8_t send_short_ir_answer(uint8_t *request );
static uint8_t send_long_ir_answer(uint8_t *request );
static uint8_t send_device_ir_info(uint8_t *request );
static void read_calibrate_data(uint8_t *request);
static void write_calibrate_data(uint8_t *request);
static void read_coil_status(uint8_t* receive);
static void force_single_coil(uint8_t* receive);
static void send_error_code(uint8_t* receive, uint8_t err_code);
static void send(uint8_t *chptr, uint8_t size);

void recieve_frame(uint8_t size){
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
    
    switch (request[FUNCTION]){
        case READ_COILS:
        {
            read_coil_status(request);
            break;
        }
        case READ_INPUT_REGISTERS:
        {
            read_input_registers(request);
            break;
        }
        case READ_HOLDING_REGISTERS:
        {
            //чтение коэффициентов калибровки
            read_calibrate_data(request);
            break;
        }
        case WRITE_SINGLE_COIL:
        {
            force_single_coil(request);
            break;
        }
        case WRITE_REGISERS:
        {
            //запись коэффициентов калибровки
            write_calibrate_data(request);
            break;
        }
        default:
        {
            send_error_code(request, ERROR_WRONG_FUNCTION);
            break;
        }
    }
}
static void read_coil_status(uint8_t *request){
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] != 0 ||
        request[STARTING_ADDRESS_HI] != 0 ||
        request[QUANTITY_OF_COILS_LO] != 4 ||
        request[QUANTITY_OF_COILS_HI] != 0)
    {
        send_error_code(request, ERROR_WRONG_REGISTER);
        return;
    }
    
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_COILS;
    response[BYTE_COUNT] = 0x01;
    response[COILS_STATUS] = 0x00 | (relay_coil_state<<4) | (Ground<<3) | (ON_500V_Minus<<2) | (ON_500V_Plus<<1) | (OFF_500V<<0);
    
    tempCRC = CRC16(response, 4);
    response[4] = (uint8_t)(tempCRC);
    response[5] = (uint8_t)(tempCRC >> 8);
    send(response, 6);
}
static void force_single_coil(uint8_t *request){
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] > 4 ||
        request[STARTING_ADDRESS_HI] != 0 )
    {
        send_error_code(request, ERROR_WRONG_REGISTER);
        return;
    }
    switch(request[STARTING_ADDRESS_LO])
    {
        case ON_RELE_COIL:
        {
            switch_relay_coil( request[COIL_VALUE_HI] ) ;
            break;
        }    
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
            send_error_code(request, ERROR_WRONG_REGISTER);
            return;
        }
    }
    send(request, 8);
}
static void read_input_registers(uint8_t *request){
    if ( 1 == send_short_ir_answer( request) )
        return;
    if ( 1 == send_long_ir_answer(request) )
        return;
    if ( 1 == send_device_ir_info(request) )
        return;
    send_error_code(request, ERROR_WRONG_REGISTER); 
}
static uint8_t send_device_ir_info(uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] < 0x64 ||
        request[STARTING_ADDRESS_LO] > 0x6A ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 7 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0x64;
    
    union REG hard_ver, serial_number_hight, serial_number_low, soft_ver;
    eeprom_read_object( 0x100, &hard_ver.w, sizeof(uint16_t) );
    eeprom_read_object( 0x102, &serial_number_hight.w, sizeof(uint16_t) );
    eeprom_read_object( 0x104, &serial_number_low.w, sizeof(uint16_t) );
    eeprom_read_object( 0x106, &soft_ver.w, sizeof(uint16_t) );

    union REG last_calibrate, next_calibrate, number_of_calibrate;
    eeprom_read_object( 0x108, &last_calibrate.w, sizeof(uint16_t) );
    eeprom_read_object( 0x10a, &next_calibrate.w, sizeof(uint16_t) );
    eeprom_read_object( 0x10c, &number_of_calibrate.w, sizeof(uint16_t) );
    
    hard_ver.w = 0x01;
    serial_number_hight.w = 0x210;
    serial_number_low.w = 0xfff3;
    soft_ver.w = 12;
    last_calibrate.w = 1219;
    next_calibrate.w = 1222;
    number_of_calibrate.w = 2;
            
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
    response[11] = last_calibrate.b[0];
    response[12] = last_calibrate.b[1];
    response[13] = next_calibrate.b[0];
    response[14] = next_calibrate.b[1];
    response[15] = number_of_calibrate.b[0];
    response[16] = number_of_calibrate.b[1];
    
    uint8_t i, j;
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }

    tempCRC = CRC16(response, j+1);
    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);
    send(response, j+3);
    return 1;
}
static uint8_t send_short_ir_answer(uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] < 0x01 ||
        request[STARTING_ADDRESS_LO] > 0x02 ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 2 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0 )
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0x01;    
    
    union REG volt, resist;
    resist.w = (uint16_t) abs(response_measure.resistance) * 1000;    //MOm -> KOm
    volt.w = (uint16_t) abs(response_measure.voltagein) * 100;
    
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = volt.b[0];
    response[4] = volt.b[1];
    response[5] = resist.b[0];
    response[6] = resist.b[1];

    uint8_t i, j;
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }

    tempCRC = CRC16(response, j+1);
    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);
    send(response, j+3);
    return 1;
}
static uint8_t send_long_ir_answer ( uint8_t *request ){
    
    if (request[STARTING_ADDRESS_LO] < 0x14 ||
        request[STARTING_ADDRESS_LO] > 0x1a ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 8 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] = request[STARTING_ADDRESS_LO] - 0x14;
       
    union FloatChar resistance, voltage, current, voltagein;
    resistance.fl = response_measure.resistance;
    voltage.fl = response_measure.voltage;
    current.fl = response_measure.current;
    voltagein.fl = response_measure.voltagein;
        
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = voltagein.ch[0];
    response[4] = voltagein.ch[1];
    response[5] = voltagein.ch[2];
    response[6] = voltagein.ch[3];
    response[7] = resistance.ch[0];
    response[8] = resistance.ch[1];
    response[9] = resistance.ch[2];
    response[10] = resistance.ch[3];
    response[11] = current.ch[0];
    response[12] = current.ch[1];
    response[13] = current.ch[2];
    response[14] = current.ch[3];
    response[15] = voltage.ch[0];
    response[16] = voltage.ch[1];
    response[17] = voltage.ch[2];
    response[18] = voltage.ch[3];
    
    uint8_t i, j;
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }
    
    //3 + request[STARTING_ADDRESS_LO]*2; // откуда
    //3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]; // докуда
    tempCRC = CRC16(response, j+1);
    response[j+1] = (uint8_t)(tempCRC);
    response[j+2] = (uint8_t)(tempCRC >> 8);
    send(response, j+3);
    return 1;
}
static void read_calibrate_data(uint8_t *request) {
    if (request[STARTING_ADDRESS_LO] != 0x01 ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] != 18 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        send_error_code(request, ERROR_WRONG_REGISTER);
        return;
    }

    union FloatChar Upole_coef, Upole_bias, Rpole_coef, Rpole_bias;
    eeprom_read_object(0x00, &Upole_coef.fl, sizeof(float));
    eeprom_read_object(0x04, &Upole_bias.fl, sizeof(float));        
    eeprom_read_object(0x08, &Rpole_coef.fl, sizeof(float));
    eeprom_read_object(0x0c, &Rpole_bias.fl, sizeof(float));

    union FloatChar Uion_coef, Uion_bias, Iion_coef, Iion_bias, Iion_coef_less100;
    eeprom_read_object(0x10, &Uion_coef.fl, sizeof(float));
    eeprom_read_object(0x14, &Uion_bias.fl, sizeof(float));        
    eeprom_read_object(0x18, &Iion_coef.fl, sizeof(float));
    eeprom_read_object(0x1c, &Iion_bias.fl, sizeof(float));
    eeprom_read_object(0x20, &Iion_coef_less100.fl, sizeof(float));
  
    response[ADDRESS] = get_addr();
    response[FUNCTION] = READ_INPUT_REGISTERS;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    response[3] = Upole_coef.ch[0];
    response[4] = Upole_coef.ch[1];
    response[5] = Upole_coef.ch[2];
    response[6] = Upole_coef.ch[3];
    response[7] = Upole_bias.ch[0];
    response[8] = Upole_bias.ch[1];
    response[9] = Upole_bias.ch[2];
    response[10] = Upole_bias.ch[3];
    response[11] = Rpole_coef.ch[0];
    response[12] = Rpole_coef.ch[1];
    response[13] = Rpole_coef.ch[2];
    response[14] = Rpole_coef.ch[3];
    response[15] = Rpole_bias.ch[0];
    response[16] = Rpole_bias.ch[1];
    response[17] = Rpole_bias.ch[2];
    response[18] = Rpole_bias.ch[3];
    response[19] = Uion_coef.ch[0];
    response[20] = Uion_coef.ch[1];
    response[21] = Uion_coef.ch[2];
    response[22] = Uion_coef.ch[3];    
    response[23] = Uion_bias.ch[0];
    response[24] = Uion_bias.ch[1];
    response[25] = Uion_bias.ch[2];
    response[26] = Uion_bias.ch[3];    
    response[27] = Iion_coef.ch[0];
    response[28] = Iion_coef.ch[1];
    response[29] = Iion_coef.ch[2];
    response[30] = Iion_coef.ch[3];    
    response[31] = Iion_bias.ch[0];
    response[32] = Iion_bias.ch[1];
    response[33] = Iion_bias.ch[2];
    response[34] = Iion_bias.ch[3];    
    response[35] = Iion_coef_less100.ch[0];
    response[36] = Iion_coef_less100.ch[1];
    response[37] = Iion_coef_less100.ch[2];
    response[38] = Iion_coef_less100.ch[3];       

    tempCRC = CRC16(response, 39);
    response[39] = (uint8_t)(tempCRC);
    response[40] = (uint8_t)(tempCRC >> 8);
    send(response, 41 );
}
static void write_calibrate_data(uint8_t *request){
    
    if (request[STARTING_ADDRESS_LO] != 0x01 ||
        request[STARTING_ADDRESS_HI] != 0x00 ||
        request[QUANTITY_OF_REGISTERS_LO] != 18 ||
        request[QUANTITY_OF_REGISTERS_HI] != 0)
    {
        send_error_code(request, ERROR_WRONG_REGISTER);
    }
    //request[6]; //Кол-во байт на передаваемые регистры
    union FloatChar Upole_coef, Upole_bias, Rpole_coef, Rpole_bias;
    Upole_coef.ch[0] = request[7];
    Upole_coef.ch[1] = request[8];
    Upole_coef.ch[2] = request[9];
    Upole_coef.ch[3] = request[10];    
    Upole_bias.ch[0] = request[11];
    Upole_bias.ch[1] = request[12];
    Upole_bias.ch[2] = request[13];
    Upole_bias.ch[3] = request[14];    
    Rpole_coef.ch[0] = request[15];
    Rpole_coef.ch[1] = request[16];
    Rpole_coef.ch[2] = request[17];
    Rpole_coef.ch[3] = request[18];    
    Rpole_bias.ch[0] = request[19];
    Rpole_bias.ch[1] = request[20];
    Rpole_bias.ch[2] = request[21];
    Rpole_bias.ch[3] = request[22];    
    eeprom_write_object(0x00, &Upole_coef.fl, sizeof(float));
    eeprom_write_object(0x04, &Upole_bias.fl, sizeof(float));        
    eeprom_write_object(0x08, &Rpole_coef.fl, sizeof(float));
    eeprom_write_object(0x0c, &Rpole_bias.fl, sizeof(float));

    union FloatChar Uion_coef, Uion_bias, Iion_coef, Iion_bias, Iion_coef_less100;
    Uion_coef.ch[0] = request[23];
    Uion_coef.ch[1] = request[24];
    Uion_coef.ch[2] = request[25];
    Uion_coef.ch[3] = request[26];
    Uion_bias.ch[0] = request[27];
    Uion_bias.ch[1] = request[28];
    Uion_bias.ch[2] = request[29];
    Uion_bias.ch[3] = request[30];
    Iion_coef.ch[0] = request[31];
    Iion_coef.ch[1] = request[32];
    Iion_coef.ch[2] = request[33];
    Iion_coef.ch[3] = request[34];
    Iion_bias.ch[0] = request[35];
    Iion_bias.ch[1] = request[36];
    Iion_bias.ch[2] = request[37];
    Iion_bias.ch[3] = request[38];
    Iion_coef_less100.ch[0] = request[39];
    Iion_coef_less100.ch[1] = request[40];
    Iion_coef_less100.ch[2] = request[41];
    Iion_coef_less100.ch[3] = request[42];
    eeprom_write_object(0x10, &Uion_coef.fl, sizeof(float));
    eeprom_write_object(0x14, &Uion_bias.fl, sizeof(float));        
    eeprom_write_object(0x18, &Iion_coef.fl, sizeof(float));
    eeprom_write_object(0x1c, &Iion_bias.fl, sizeof(float));
    eeprom_write_object(0x20, &Iion_coef_less100.fl, sizeof(float));
    
    response[ADDRESS] = get_addr();
    response[FUNCTION] = WRITE_REGISERS;
    response[2] = request[STARTING_ADDRESS_HI];
    response[3] = request[STARTING_ADDRESS_LO];
    response[4] = request[QUANTITY_OF_REGISTERS_HI];
    response[5] = request[QUANTITY_OF_REGISTERS_LO];

    tempCRC = CRC16(response, 6);
    response[6] = (uint8_t)(tempCRC);
    response[7] = (uint8_t)(tempCRC >> 8);
    send(response, 8);
}
static void send_error_code(uint8_t *request, uint8_t error_code){
    response[ADDRESS] = get_addr();
    response[1] = (request[FUNCTION] | 0x80);
    response[2] = error_code;
    
    uint16_t tempCRC = CRC16(response, 3 );
    response[3] = (uint8_t)(tempCRC);
    response[4] = (uint8_t)(tempCRC >> 8);
    send(response, 5);
}
static void send(uint8_t *chptr, uint8_t size){
    uint8_t i;
    TX_nRC_SetHigh();//переключаем на передачу
    for (i = 0; i < size; i++){
        EUSART1_Write(*chptr++);
        //__delay_us(100);
    }
    SetTXState();
}
void send_done(void){
    TX_nRC_SetLow();//переключаем на приём
}
void save_measure(measures measure){
    // Disable the Global Interrupts
    INTERRUPT_GlobalInterruptHighDisable();//INTERRUPT_GlobalInterruptDisable();
    
    response_measure = measure;
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptHighEnable();//INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
}
void ResetTXState(void){
    TXState = false;
}
void SetTXState(void){
    TXState = true;
}
bool IsTXState(void){
    return TXState;
}
