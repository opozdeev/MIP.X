/**
  Generated Interrupt Manager Header File

 *   @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.h

  @Summary:
    This is the Interrupt Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description:
    This header file provides implementations for global interrupt handling.
    For individual peripheral handlers please see the peripheral driver for
    all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC18F14K50
        Driver Version    :  2.12
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.00 or later
        MPLAB 	          :  MPLAB X 5.10
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

#include "interrupt_manager.h"
#include "mcc.h"

void  INTERRUPT_Initialize (void)
{
    // Enable Interrupt Priority Vectors
    RCONbits.IPEN = 1;

    // Assign peripheral interrupt priority vectors

    // TMRI - high priority
    IPR1bits.TMR2IP = 1;


    // RCI - low priority
    IPR1bits.RC1IP = 0;    

    // TMRI - low priority
    INTCON2bits.TMR0IP = 0;    

    // TXI - low priority
    IPR1bits.TX1IP = 0;    

    // ADI - low priority
    IPR1bits.ADIP = 0;    

}

void __interrupt() INTERRUPT_InterruptManagerHigh (void)
{
//    LATBbits.LATB6 = 1;//проверка частоты срабатывания
   // interrupt handler
    if(PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1)
    {
 static unsigned char cnt=0;
        //TMR2_ISR();
        PIR1bits.TMR2IF = 0;
        cnt++;
        cnt &= 0x3;
        if (cnt == 0) ADC_StartConversion();
    }
    else
    {
        //Unhandled Interrupt
    }
//    LATBbits.LATB6 = 0;//проверка частоты срабатывания
}

void __interrupt(low_priority) INTERRUPT_InterruptManagerLow (void)
{
    // interrupt handler
//    LATBbits.LATB6 = 1;//проверка частоты срабатывания
    if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    {
        EUSART1_RxDefaultInterruptHandler();
    }
    else if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    else if(PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1)
    {
        EUSART1_TxDefaultInterruptHandler();
    }
    else if(PIE1bits.ADIE == 1 && PIR1bits.ADIF == 1)
    {
//        LATCbits.LATC6 = 1;//проверка частоты срабатывания
        ADC_ISR();
//        LATCbits.LATC6 = 0;//проверка частоты срабатывания
    }
    else
    {
        //Unhandled Interrupt
    }
//    LATBbits.LATB6 = 0;//проверка частоты срабатывания
}
/**
 End of File
*/
