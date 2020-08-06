/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC18F14K50
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "measure.h"
#include "RS485.h"
#include "Indikator.h"

/*
                         Main application
 */
void main(void)
{
    extern union uIndData Indik;
    // Initialize the device
    SYSTEM_Initialize();

    //настройка регистра управления
    LATCH_SetLow();//запретим защёлкивание данных в регистр управления
    CLR_SetLow();//сбросим регистр управления
    CLR_SetHigh();//уберём сброс
    Indik.sData.AddrLatch = 0;//активируем строб записи адреса
    Indik.sData.RelayOut = NC;//обы ключа будут буз тока
    Indik.sData.Hl1 = HL1GREEN;//включим один цвет
    Indik.sData.Hl2 = HL2OFF;//включим второй светодиод
    Indik.sData.HvOut = OUTOFF;//выключим выходные реле
    WriteOuts(Indik);//запишем это в управляющий регистр и подготовимся к чтению адреса
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    measures measure;
    TX_nRC_SetLow();//переключимся на приём
    
    //read_address();
    
    while (1)
    {
        measure = get_measure();//WATCHDOG перезагружается во время работы АЦП
        save_measure(measure);
        safe_switch(measure.voltage);
        Indik.sData.Hl1 = ~Indik.sData.Hl1;//помигаем светодиодом
        WriteOuts(Indik);//обновим содержимое регистра управления и считаем адрес если нужно
        Indik.sData.AddrLatch = ~Indik.sData.AddrLatch;
        //иногда не выключается передатчик, код дальше должен выключать его, хотя я в этом не уверен до конца
        if ((!IsTXState()) && TX_nRC_GetValue()) TX_nRC_SetLow();//если передавать не надо, но передатчик включен - выключаем передатчик
        else CLRWDT();
    };
}

/**
 End of File
*/