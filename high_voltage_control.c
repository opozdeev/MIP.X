#include "high_voltage_control.h"
#include "Indikator.h"

bool OFF_500V = true;
bool ON_500V_Plus = false;
bool ON_500V_Minus = false;
bool Ground = false;

bool Transition_to_Ground = false;
bool Transition_to_OFF_500V = true;
bool Transition_to_ON_500V_Plus = false;
bool Transition_to_ON_500V_Minus = false;

void TurnON_500V_Minus(void)
{
    if (ON_500V_Minus)
        return;
    EPWM1_TurnOFF();
    
    Indik.sData.Hl2 = HL2ON;//ВКЛЮЧИТЬ СВЕТОДИОД
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = true;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    RC4_SetDigitalOutput();
    RC4_SetLow();
    RC5_SetDigitalOutput();
    RC5_SetLow();
}

void TurnON_500V_Plus(void)
{
    if (ON_500V_Plus)
        return;
    EPWM1_TurnOFF();
    
    Indik.sData.Hl2 = HL2ON;//ВКЛЮЧИТЬ СВЕТОДИОД
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = true;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    RC4_SetDigitalOutput();
    RC4_SetLow();
    RC5_SetDigitalOutput();
    RC5_SetLow();
 }

void TurnOFF_500V(void)
{
    if (OFF_500V)
        return;
    EPWM1_TurnOFF();
    RC4_SetDigitalInput();
 //   RC4_SetLow();
    RC5_SetDigitalInput();
//    RC5_SetLow();
    
    Indik.sData.Hl2 = HL2OFF;//ВЫКЛЮЧИМ СВЕТОДИОД
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = true;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
}

void TurnON_GND(void)
{
    if (Ground)
        return;
    EPWM1_TurnOFF();
    RC4_SetDigitalInput();
//    RC4_SetLow();
    RC5_SetDigitalInput();
//    RC5_SetLow();
     
    Indik.sData.Hl2 = HL2OFF;//ВЫКЛЮЧИМ СВЕТОДИОД
    
    Transition_to_Ground = true;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
}

void safe_switch(float voltage)
{
    // Disable the Global Interrupts
    INTERRUPT_GlobalInterruptLowDisable();//INTERRUPT_GlobalInterruptDisable();
    
    if (Transition_to_Ground)
    {
//        if (voltage <= 20)
        {
//!!!!!!!!!!!!!!!!!!ВЫКЛЮЧИТЬ МИНУСОВУЮ ПОЛЯРНОСТЬ            OFF_500V_M_SetLow();//включить все реле
//!!!!!!!!!!!!!!!!!!ВЫКЛЮЧИТЬ ПЛЮСОВУЮ ПОЛЯРНОСТЬ            OFF_500V_P_SetLow();
            Indik.sData.HvOut = OUTOFF;//тут идёт разряд на землю, но в данной версии его нет
            
            Transition_to_Ground = false;
            Transition_to_OFF_500V = false;
            Transition_to_ON_500V_Plus = false;
            Transition_to_ON_500V_Minus = false;
            
            Ground = true;
            OFF_500V = false;
            ON_500V_Plus = false;
            ON_500V_Minus = false;
        }
    }
    if (Transition_to_OFF_500V)
    {
//        if (voltage <= 20)
        {
//!!!!!!!!!!!!!!!!!!            OFF_500V_P_SetHigh();//выключить все реле
//!!!!!!!!!!!!!!!!!!            OFF_500V_M_SetHigh();
            Indik.sData.HvOut = OUTOFF;//тут идёт разряд на землю, но в данной версии его нет
            
            Transition_to_Ground = false;
            Transition_to_OFF_500V = false;
            Transition_to_ON_500V_Plus = false;
            Transition_to_ON_500V_Minus = false;
            
            OFF_500V = true;
            ON_500V_Plus = false;
            ON_500V_Minus = false;
            Ground = false;
        }
    }
    
    if (Transition_to_ON_500V_Plus)
    {
//        if (voltage <= 20)
        {
//!!!!!!!!!!!!!!!!            OFF_500V_M_SetHigh(); 
//!!!!!!!!!!!!!!!!            OFF_500V_P_SetLow();
            Indik.sData.HvOut = OUTPLUS;//включить плюсовую полярность
            EPWM1_TurnON();
            
            Transition_to_Ground = false;
            Transition_to_OFF_500V = false;
            Transition_to_ON_500V_Plus = false;
            Transition_to_ON_500V_Minus = false;
            
            Ground = false;
            OFF_500V = false;
            ON_500V_Plus = true;
            ON_500V_Minus = false;
        }
    }
    if (Transition_to_ON_500V_Minus)
    {
//        if (voltage <= 20)
        {
//!!!!!!!!!!!!!!!!!            OFF_500V_P_SetHigh(); 
//!!!!!!!!!!!!!!!!!            OFF_500V_M_SetLow();
            Indik.sData.HvOut = OUTMINUS;//включить минусовую полярность
            EPWM1_TurnON();
            
            Transition_to_Ground = false;
            Transition_to_OFF_500V = false;
            Transition_to_ON_500V_Plus = false;
            Transition_to_ON_500V_Minus = false;
            
            OFF_500V = false;
            ON_500V_Plus = false;
            ON_500V_Minus = true;
            Ground = false;
        }
    }
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptLowEnable();//INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();
}
