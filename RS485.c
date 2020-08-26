#include "RS485.h"
#include "relay_control.h"

#include "high_voltage_control.h"
#include "address.h"
#include "frame_types.h"
#include "CRC16.h"
#include "mcc_generated_files/memory.h"
#include "version.h"

#define READ_COILS                  0x01
#define READ_HOLDING_REGISTERS      0x03
#define READ_INPUT_REGISTERS        0x04
#define WRITE_SINGLE_COIL           0x05
#define WRITE_REGISERS              0x10

#define ERROR_WRONG_FUNCTION        0x01
#define ERROR_WRONG_REGISTER        0x02
#define ERROR_WRONG_REQUEST_DATA    0x03
#define ERROR_WRONG_INTERNAL_DATA   0x04

#define MAX_BYTE_IN_POLE_NAME       30    

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
 * 0xE8,0xE9  <- 100  номер серии/версия харда (печатной платы/принципиальной схемы)
 * 0xEA,0xEB  <- 101  порядковый номер в серии (старшие два байта)
 * 0xEC,0xED  <- 102  порядковый номер в серии (младшие два байта)
 * 0xEE,0xEF  <- 104  месяц последней калибровки  (1219 -> декабрь 2019)
 * 0xF0,0xF1  <- 105  месяц следущей калибровки (1223 -> декабрь 2023)
 * 0xF2,0xF2  <- 106  общее количесво пройденых калибровок
 
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
/*
 * Имя полюса измеряемое данным МИП, храняться в Holding Registers
 * Регистр     |    Описание                                | Адрес eeprom 
 * 200 (0xC8)  | Количество байт в строке в кодировке utf8  | 0xC8
 * 201 (0xC8)  | Байты строки (максимум 30 байт)            | 0xC9 - 0xE7
 * Примечание
 * 30 байт - это 15 кирилических символов в utf8 
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
static uint8_t response[42]; 
static uint8_t i, j;

static void read_input_registers(uint8_t* receive);
static uint8_t send_short_ir_answer(uint8_t *request );
static uint8_t send_long_ir_answer(uint8_t *request );
static uint8_t send_device_ir_info(uint8_t *request );
static void read_holding_registers(uint8_t* receive);
static uint8_t read_pole_name(uint8_t *request );
static uint8_t read_calibrate_data(uint8_t *request);
static void write_registers(uint8_t *request);
static uint8_t write_calibrate_data(uint8_t *request);
static uint8_t write_pole_name(uint8_t *request);
static uint8_t write_mip_info(uint8_t *request);
static void read_coil_status(uint8_t* receive);
static void force_single_coil(uint8_t* receive);
static void send_error_code(uint8_t* receive, uint8_t err_code);
static void send(uint8_t *chptr, uint8_t size);

void recieve_frame(uint8_t size){
    uint8_t request[EUSART1_RX_BUFFER_SIZE];
    
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
    response[ADDRESS] = request[ADDRESS];
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
            read_holding_registers(request);
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
            write_registers(request);
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
        request[QUANTITY_OF_COILS_LO] != 4 )
    {
        send_error_code(request, ERROR_WRONG_REGISTER);
        return;
    }
    
    response[FUNCTION] = READ_COILS;
    response[BYTE_COUNT] = 0x01;
    response[COILS_STATUS] = 0x00 | (relay_coil_state<<4) | (Ground<<3) | (ON_500V_Minus<<2) | (ON_500V_Plus<<1) | (OFF_500V<<0);
    send(response, 6);
}
static void force_single_coil(uint8_t *request){
    //проверим адекватность запроса
    if (request[STARTING_ADDRESS_LO] > 4  )
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
    /*тут не было расчента crc он по идее должен будет сейчас пересчитать crc*/
    send(request, 8);
}
static void read_input_registers(uint8_t *request){
    response[FUNCTION] = READ_INPUT_REGISTERS;
    if ( 1 == send_short_ir_answer( request) )
        return;
    if ( 1 == send_long_ir_answer(request) )
        return;
    if ( 1 == send_device_ir_info(request) )
        return;
    send_error_code(request, ERROR_WRONG_REGISTER); 
}

static uint8_t split_response(uint8_t const *request, uint8_t *response){
    //3 + request[STARTING_ADDRESS_LO]*2; // откуда
    //3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]; // докуда
    j = 2;
    for(i = 3 + request[STARTING_ADDRESS_LO]*2; i < 3 + request[STARTING_ADDRESS_LO]*2 + request[QUANTITY_OF_REGISTERS_LO]*2; i++)
    {
        j++;
        response[j] = response[i];
    }
    return j;
}
static uint8_t send_device_ir_info(uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] < 0x64 ||
        request[STARTING_ADDRESS_LO] > 0x6A ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 7 )
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] -= 0x64;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    
    union REG hard_ver, serial_number_hight, serial_number_low, soft_ver;
    eeprom_read_object( 0xE8, &hard_ver.w, sizeof(uint16_t) );
    eeprom_read_object( 0xEA, &serial_number_hight.w, sizeof(uint16_t) );
    eeprom_read_object( 0xEC, &serial_number_low.w, sizeof(uint16_t) );
    soft_ver.w = get_version ();
    
    union REG last_calibrate, next_calibrate, number_of_calibrate;
    eeprom_read_object( 0xEE, &last_calibrate.w, sizeof(uint16_t) );
    eeprom_read_object( 0xF0, &next_calibrate.w, sizeof(uint16_t) );
    eeprom_read_object( 0xF2, &number_of_calibrate.w, sizeof(uint16_t) );
    
    response[3] = hard_ver.b[0];
    response[4] = hard_ver.b[1];
    response[5] = serial_number_hight.b[0];
    response[6] = serial_number_hight.b[1];
    response[7] = serial_number_low.b[0];
    response[8] = serial_number_low.b[1];
    response[9] = soft_ver.b[1];
    response[10] = soft_ver.b[0];
    response[11] = last_calibrate.b[0];
    response[12] = last_calibrate.b[1];
    response[13] = next_calibrate.b[0];
    response[14] = next_calibrate.b[1];
    response[15] = number_of_calibrate.b[0];
    response[16] = number_of_calibrate.b[1];

    j = split_response (request, response );
    
    send(response, j+3);
    return 1;
}
static uint8_t send_short_ir_answer(uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] < 0x01 ||
        request[STARTING_ADDRESS_LO] > 0x02 ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 2  )
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] -= 0x01;   
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;
    
    union REG volt, resist;
    resist.w = (uint16_t) abs(response_measure.resistance) * 1000;    //MOm -> KOm
    volt.w = (uint16_t) abs(response_measure.voltagein) * 100;
    
    response[3] = volt.b[1];
    response[4] = volt.b[0];
    response[5] = resist.b[1];
    response[6] = resist.b[0];

    j = split_response (request, response );
    
    send(response, j+3);
    return 1;
}
static uint8_t send_long_ir_answer ( uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] < 0x14 ||
        request[STARTING_ADDRESS_LO] > 0x1a ||
        request[QUANTITY_OF_REGISTERS_LO] < 1 ||
        request[QUANTITY_OF_REGISTERS_LO] > 8 )
    {
        return 0;
    }
    request[STARTING_ADDRESS_LO] -= 0x14;
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;

    union FloatChar fltTmp;
    fltTmp.fl = response_measure.voltagein;
    response[3] = fltTmp.ch[3];
    response[4] = fltTmp.ch[2];
    response[5] = fltTmp.ch[1];
    response[6] = fltTmp.ch[0];
    
    fltTmp.fl = response_measure.resistance;
    response[7] = fltTmp.ch[3];
    response[8] = fltTmp.ch[2];
    response[9] = fltTmp.ch[1];
    response[10] = fltTmp.ch[0];

    fltTmp.fl = response_measure.current;
    response[11] = fltTmp.ch[3];
    response[12] = fltTmp.ch[2];
    response[13] = fltTmp.ch[1];
    response[14] = fltTmp.ch[0];
    
    fltTmp.fl = response_measure.voltage;
    response[15] = fltTmp.ch[3];
    response[16] = fltTmp.ch[2];
    response[17] = fltTmp.ch[1];
    response[18] = fltTmp.ch[0];
    
    j = split_response (request, response );
 
    send(response, j+3);
    return 1;
}
static void read_holding_registers( uint8_t *request ){

    response[FUNCTION] = READ_HOLDING_REGISTERS;
    if ( 1 == read_calibrate_data(request) )
        return;
    if ( 1 == read_pole_name(request) )
        return;
    send_error_code(request, ERROR_WRONG_REGISTER);
}
static uint8_t read_pole_name( uint8_t *request ){
    if (request[STARTING_ADDRESS_LO] != 0xC8 ||
        request[QUANTITY_OF_REGISTERS_LO] > 15 )
    {
        return 0;
    }
    
    uint8_t byte_in_name = 0;
    eeprom_read_object(0xC8, &byte_in_name, 1 );
    if (byte_in_name > MAX_BYTE_IN_POLE_NAME){
        send_error_code(request, ERROR_WRONG_INTERNAL_DATA);    
        return 1;    
    }
    uint8_t byte_count = request[QUANTITY_OF_REGISTERS_LO]*2;
    response[BYTE_COUNT] = byte_count;
    
    eeprom_read_object(0xC9, &response[3], byte_in_name );
        
    for (i=byte_in_name; i < byte_count; ++i ){
        response[3+i] = 0x00;
    }
    send(response, byte_count+5 );
    return 1;    
}
static uint8_t read_calibrate_data(uint8_t *request) {
    if (request[STARTING_ADDRESS_LO] != 0x01 ||
        request[QUANTITY_OF_REGISTERS_LO] != 18 )
    {
        return 0;
    }
    response[BYTE_COUNT] = request[QUANTITY_OF_REGISTERS_LO] * 2;    
    eeprom_read_object(0x00, &response[3], sizeof(float)); //Upole_coef
    eeprom_read_object(0x04, &response[7], sizeof(float)); //Upole_bias        
    eeprom_read_object(0x08, &response[11], sizeof(float)); //Rpole_coef
    eeprom_read_object(0x0c, &response[15], sizeof(float)); //Rpole_bias

    eeprom_read_object(0x10, &response[19], sizeof(float)); //Uion_coef
    eeprom_read_object(0x14, &response[23], sizeof(float)); //Uion_bias        
    eeprom_read_object(0x18, &response[27], sizeof(float)); //Iion_coef
    eeprom_read_object(0x1c, &response[31], sizeof(float)); //Iion_bias 
    eeprom_read_object(0x20, &response[35], sizeof(float)); //Iion_coef_less100            
    send(response, 41 );
    return 1;
}
static void write_registers(uint8_t *request){
    response[FUNCTION] = WRITE_REGISERS; 
    if (1 == write_calibrate_data(request) )
        return;
    if (1 == write_pole_name( request ) )
        return;    
    if (1 == write_mip_info( request ) )
        return;    
    send_error_code(request, ERROR_WRONG_REGISTER);    
}
static uint8_t write_mip_info(uint8_t *request){
       
    if (request[STARTING_ADDRESS_LO] != 0x64 ||
        request[QUANTITY_OF_REGISTERS_LO] != 3 )
    {
        return 0;
    }
  
    eeprom_write_object( 0xE8, &request[7], sizeof(uint16_t) ); //номер серии/версия харда (печатной платы/принципиальной схемы)
    eeprom_write_object( 0xEA, &request[9], sizeof(uint16_t) ); //порядковый номер в серии (старшие два байта)
    eeprom_write_object( 0xEC, &request[11], sizeof(uint16_t) ); //порядковый номер в серии (младшие два байта)
    
    response[2] = 0x00; //так как отказались от проверки STARTING_ADDRESS_HI на равенство нулю, в угоду оптимизации
    response[3] = request[STARTING_ADDRESS_LO];
    response[4] = 0x00; //так как отказались от проверки STARTING_ADDRESS_HI на равенство нулю, в угоду оптимизации
    response[5] = request[QUANTITY_OF_REGISTERS_LO];
    
    send(response, 8);
    return 1;
}
static uint8_t write_pole_name(uint8_t *request){
    if (request[STARTING_ADDRESS_LO] != 0xC8 ||
        request[QUANTITY_OF_REGISTERS_LO] > 15 )
    {
        return 0;
    }
   
    //200 (0xC8) - количество байт в строке
    //201-231 (0xC9 - E7)   -   зарезервированная область для хранения строки
    //30 - максимум строка может содержать 30 байт, - это 15 кирилических символов в utf8
    if (request[6] > MAX_BYTE_IN_POLE_NAME ){
        send_error_code(request, ERROR_WRONG_REQUEST_DATA);    
        return 1;    
    }
        
    eeprom_write_object(0xC8, &request[6], 1 );
    eeprom_write_object(0xC9, &request[7], request[6] );
    //__delay_us(2000);
       
    response[2] = 0x00;
    response[3] = request[STARTING_ADDRESS_LO];
    response[4] = 0x00;
    response[5] = request[QUANTITY_OF_REGISTERS_LO];

    return 1;
}
static uint8_t write_calibrate_data(uint8_t *request){
    
    if (request[STARTING_ADDRESS_LO] != 0x01 ||
        request[QUANTITY_OF_REGISTERS_LO] != 21 )
    {
        return 0;
    }
    //request[6]; //Кол-во байт на передаваемые регистры

    eeprom_write_object(0x00, &request[7], sizeof(float));  //Upole_coef
    eeprom_write_object(0x04, &request[11], sizeof(float)); //Upole_bias        
    eeprom_write_object(0x08, &request[15], sizeof(float)); //Rpole_coef
    eeprom_write_object(0x0c, &request[19], sizeof(float)); //Rpole_bias

    eeprom_write_object(0x10, &request[23], sizeof(float)); //Uion_coef
    eeprom_write_object(0x14, &request[27], sizeof(float)); //Uion_bias        
    eeprom_write_object(0x18, &request[31], sizeof(float)); //Iion_coef 
    eeprom_write_object(0x1c, &request[35], sizeof(float)); //Iion_bias 
    eeprom_write_object(0x20, &request[39], sizeof(float)); //Iion_coef_less100   
    eeprom_write_object( 0xEE, &request[43], sizeof(uint16_t) ); //месяц последней калибровки  (1219 -> декабрь 2019)
    eeprom_write_object( 0xF0, &request[45], sizeof(uint16_t) ); //месяц следущей калибровки (1223 -> декабрь 2023)
    eeprom_write_object( 0xF2, &request[47], sizeof(uint16_t) ); //общее количесво пройденых калибровок
    
    response[2] = 0x00;
    response[3] = request[STARTING_ADDRESS_LO];
    response[4] = 0x00;
    response[5] = request[QUANTITY_OF_REGISTERS_LO];

    send(response, 8);
    return 1;
}
static void send_error_code(uint8_t *request, uint8_t error_code){
    response[1] = (request[FUNCTION] | 0x80);
    response[2] = error_code;
    send(response, 5);
}
static void send(uint8_t *chptr, uint8_t size){
    /*Расчитаем CRC для пакета данных*/
    uint16_t tempCRC = CRC16(response, size-2 );
    response[size-2] = (uint8_t)(tempCRC);
    response[size-1] = (uint8_t)(tempCRC >> 8);
    /*Произведем передачу*/
    TX_nRC_SetHigh();
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
    INTERRUPT_GlobalInterruptLowDisable();//INTERRUPT_GlobalInterruptDisable();
    
    response_measure = measure;
 
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptLowEnable();//INTERRUPT_GlobalInterruptEnable();
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
