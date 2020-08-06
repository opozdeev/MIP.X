/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC18F14K50
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.00
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set CLR aliases
#define CLR_TRIS                 TRISAbits.TRISA4
#define CLR_LAT                  LATAbits.LATA4
#define CLR_PORT                 PORTAbits.RA4
#define CLR_WPU                  WPUAbits.WPUA4
#define CLR_ANS                  ANSELbits.ANS3
#define CLR_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define CLR_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define CLR_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define CLR_GetValue()           PORTAbits.RA4
#define CLR_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define CLR_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define CLR_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define CLR_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define CLR_SetAnalogMode()      do { ANSELbits.ANS3 = 1; } while(0)
#define CLR_SetDigitalMode()     do { ANSELbits.ANS3 = 0; } while(0)

// get/set LATCH aliases
#define LATCH_TRIS                 TRISAbits.TRISA5
#define LATCH_LAT                  LATAbits.LATA5
#define LATCH_PORT                 PORTAbits.RA5
#define LATCH_WPU                  WPUAbits.WPUA5
#define LATCH_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define LATCH_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define LATCH_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define LATCH_GetValue()           PORTAbits.RA5
#define LATCH_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define LATCH_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define LATCH_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define LATCH_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set FB_I aliases
#define FB_I_TRIS                 TRISBbits.TRISB4
#define FB_I_LAT                  LATBbits.LATB4
#define FB_I_PORT                 PORTBbits.RB4
#define FB_I_WPU                  WPUBbits.WPUB4
#define FB_I_ANS                  ANSELHbits.ANS10
#define FB_I_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define FB_I_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define FB_I_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define FB_I_GetValue()           PORTBbits.RB4
#define FB_I_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define FB_I_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define FB_I_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define FB_I_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define FB_I_SetAnalogMode()      do { ANSELHbits.ANS10 = 1; } while(0)
#define FB_I_SetDigitalMode()     do { ANSELHbits.ANS10 = 0; } while(0)

// get/set RB5 procedures
#define RB5_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define RB5_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define RB5_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define RB5_GetValue()              PORTBbits.RB5
#define RB5_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define RB5_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define RB5_SetPullup()             do { WPUBbits.WPUB5 = 1; } while(0)
#define RB5_ResetPullup()           do { WPUBbits.WPUB5 = 0; } while(0)
#define RB5_SetAnalogMode()         do { ANSELHbits.ANS11 = 1; } while(0)
#define RB5_SetDigitalMode()        do { ANSELHbits.ANS11 = 0; } while(0)

// get/set SCK aliases
#define SCK_TRIS                 TRISBbits.TRISB6
#define SCK_LAT                  LATBbits.LATB6
#define SCK_PORT                 PORTBbits.RB6
#define SCK_WPU                  WPUBbits.WPUB6
#define SCK_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define SCK_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define SCK_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define SCK_GetValue()           PORTBbits.RB6
#define SCK_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define SCK_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define SCK_SetPullup()          do { WPUBbits.WPUB6 = 1; } while(0)
#define SCK_ResetPullup()        do { WPUBbits.WPUB6 = 0; } while(0)

// get/set RB7 procedures
#define RB7_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define RB7_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define RB7_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define RB7_GetValue()              PORTBbits.RB7
#define RB7_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define RB7_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define RB7_SetPullup()             do { WPUBbits.WPUB7 = 1; } while(0)
#define RB7_ResetPullup()           do { WPUBbits.WPUB7 = 0; } while(0)

// get/set RC0 procedures
#define RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define RC0_GetValue()              PORTCbits.RC0
#define RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define RC0_SetAnalogMode()         do { ANSELbits.ANS4 = 1; } while(0)
#define RC0_SetDigitalMode()        do { ANSELbits.ANS4 = 0; } while(0)

// get/set Vin aliases
#define Vin_TRIS                 TRISCbits.TRISC1
#define Vin_LAT                  LATCbits.LATC1
#define Vin_PORT                 PORTCbits.RC1
#define Vin_ANS                  ANSELbits.ANS5
#define Vin_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define Vin_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define Vin_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define Vin_GetValue()           PORTCbits.RC1
#define Vin_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define Vin_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define Vin_SetAnalogMode()      do { ANSELbits.ANS5 = 1; } while(0)
#define Vin_SetDigitalMode()     do { ANSELbits.ANS5 = 0; } while(0)

// get/set FB_U aliases
#define FB_U_TRIS                 TRISCbits.TRISC2
#define FB_U_LAT                  LATCbits.LATC2
#define FB_U_PORT                 PORTCbits.RC2
#define FB_U_ANS                  ANSELbits.ANS6
#define FB_U_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define FB_U_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define FB_U_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define FB_U_GetValue()           PORTCbits.RC2
#define FB_U_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define FB_U_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define FB_U_SetAnalogMode()      do { ANSELbits.ANS6 = 1; } while(0)
#define FB_U_SetDigitalMode()     do { ANSELbits.ANS6 = 0; } while(0)

// get/set ADDR aliases
#define ADDR_TRIS                 TRISCbits.TRISC3
#define ADDR_LAT                  LATCbits.LATC3
#define ADDR_PORT                 PORTCbits.RC3
#define ADDR_ANS                  ANSELbits.ANS7
#define ADDR_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define ADDR_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define ADDR_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define ADDR_GetValue()           PORTCbits.RC3
#define ADDR_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define ADDR_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define ADDR_SetAnalogMode()      do { ANSELbits.ANS7 = 1; } while(0)
#define ADDR_SetDigitalMode()     do { ANSELbits.ANS7 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)

// get/set TX_nRC aliases
#define TX_nRC_TRIS                 TRISCbits.TRISC6
#define TX_nRC_LAT                  LATCbits.LATC6
#define TX_nRC_PORT                 PORTCbits.RC6
#define TX_nRC_ANS                  ANSELHbits.ANS8
#define TX_nRC_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define TX_nRC_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define TX_nRC_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define TX_nRC_GetValue()           PORTCbits.RC6
#define TX_nRC_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define TX_nRC_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define TX_nRC_SetAnalogMode()      do { ANSELHbits.ANS8 = 1; } while(0)
#define TX_nRC_SetDigitalMode()     do { ANSELHbits.ANS8 = 0; } while(0)

// get/set SDO aliases
#define SDO_TRIS                 TRISCbits.TRISC7
#define SDO_LAT                  LATCbits.LATC7
#define SDO_PORT                 PORTCbits.RC7
#define SDO_ANS                  ANSELHbits.ANS9
#define SDO_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define SDO_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define SDO_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define SDO_GetValue()           PORTCbits.RC7
#define SDO_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define SDO_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define SDO_SetAnalogMode()      do { ANSELHbits.ANS9 = 1; } while(0)
#define SDO_SetDigitalMode()     do { ANSELHbits.ANS9 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/