#include "address.h"

static uint8_t address = 0;

uint8_t get_addr(void)
{
    return address;
}

void init_address(void)
{
    int8_t clk = 0;

    SCLK_SetLow();//SCLK = 0;
    HL2_LOAD_SetHigh();//LOAD = 1;
    HL2_LOAD_SetLow();//LOAD = 0;
    HL2_LOAD_SetHigh();//LOAD = 1;

    address = 0;
    address = address | (ADDR_GetValue() << clk);
  
    for (clk = 1; clk < 8; clk++)
    {
        SCLK_SetHigh();//SCLK = 1;
        SCLK_SetLow();//SCLK = 0;
        address = address | (ADDR_GetValue() << clk);
    }
    address ^= 0xFF; //инвертируем биты, т.к. включенный переключатель подтягивает к нулю 
}