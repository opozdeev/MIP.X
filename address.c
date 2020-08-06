#include "address.h"

static uint8_t address = 0;

uint8_t get_addr(void)
{
    return address;
}

void read_address(void)
{
    int8_t clk = 0;

    SCK_SetLow();
    //!!!!!!!!!!!!!!!!!!!!!!!Загрузить адрес передёрнув в ноль вывод LD на DD3
/*    HL2_LOAD_SetHigh();//LOAD = 1;
    HL2_LOAD_SetLow();//LOAD = 0;
    HL2_LOAD_SetHigh();//LOAD = 1;
 */

    address = 0;
    address = address | (ADDR_GetValue() << clk);
  
    for (clk = 1; clk < 8; clk++)
    {
        SCK_SetHigh();//SCLK = 1;
        SCK_SetLow();//SCLK = 0;
        address = address | (ADDR_GetValue() << clk);
    }
    address = 0;//!!!!!!!!!!!!!!!!!!!!!!!! Адрес 255
    address ^= 0xFF; //инвертируем биты, т.к. включенный переключатель подтягивает к нулю 
}