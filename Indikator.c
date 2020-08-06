#include "mcc_generated_files/mcc.h"
#include "Indikator.h"

union uIndData Indik;
static unsigned char Address = 0;
//------------------------------------------------------------------------------
// читает адрес если нужно и записывает его в Address, выводит данные управляющий регистр
bool WriteOuts(union uIndData Data)
{
    unsigned char i, tmpAddress = 0;
    bool NeedReadAddr = Data.sData.AddrLatch;//флаг необходимости чтения адреса
    
    for (i=0; i < 8; i++)
    {
        if (!NeedReadAddr)//если нужно читать адрес
        {
            tmpAddress <<= 1;
            tmpAddress |= 0x1 & ADDR_GetValue();//считать вход адреса
        }
        SCK_SetLow();
        SDO_LAT = Data.ucData & 1;
        Data.ucData >>= 1;
        __delay_us(10);
        SCK_SetHigh();
        __delay_us(10);
    }
    SCK_SetLow();
    LATCH_SetHigh();
    __delay_us(10);
    LATCH_SetLow();
    if (!NeedReadAddr) Address = ~tmpAddress;//если нужно читать адрес
    return !NeedReadAddr;//переключим состояние чтения адреса
}
//------------------------------------------------------------------------------
//возвращает адрес
unsigned char get_addr(void)
{
    return Address;
}
