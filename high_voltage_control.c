#include "high_voltage_control.h"

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
    //OFF_500V_P_SetHigh(); 
    //OFF_500V_M_SetLow();
    //EPWM1_TurnON();
    if (ON_500V_Minus)
        return;
    EPWM1_TurnOFF();
    
    HL2_LOAD_SetLow();
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = true;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    /*OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = true;
    Ground = false;*/
}

void TurnON_500V_Plus(void)
{
    //OFF_500V_M_SetHigh(); 
    //OFF_500V_P_SetLow();
    //EPWM1_TurnON();
    if (ON_500V_Plus)
        return;
    EPWM1_TurnOFF();
    
    HL2_LOAD_SetLow();
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = true;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    /*OFF_500V = false;
    ON_500V_Plus = true;
    ON_500V_Minus = false;
    Ground = false;*/
}

void TurnOFF_500V(void)
{
    if (OFF_500V)
        return;
    EPWM1_TurnOFF();
    RC4_SetDigitalOutput();
    RC4_SetLow();
    RC5_SetDigitalOutput();
    RC5_SetLow();
    //OFF_500V_P_SetHigh(); 
    //OFF_500V_M_SetHigh();
    
    HL2_LOAD_SetHigh();
    
    Transition_to_Ground = false;
    Transition_to_OFF_500V = true;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    /*OFF_500V = true;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;*/
}

void TurnON_GND(void)
{
    if (Ground)
        return;
    EPWM1_TurnOFF();
    RC4_SetDigitalOutput();
    RC4_SetLow();
    RC5_SetDigitalOutput();
    RC5_SetLow();
    //OFF_500V_M_SetLow();
    //OFF_500V_P_SetLow();
    
    HL2_LOAD_SetHigh();
    
    Transition_to_Ground = true;
    Transition_to_OFF_500V = false;
    Transition_to_ON_500V_Plus = false;
    Transition_to_ON_500V_Minus = false;
    OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = false;
    /*OFF_500V = false;
    ON_500V_Plus = false;
    ON_500V_Minus = false;
    Ground = true;*/
}

void safe_switch(float voltage)
{
    // Disable the Global Interrupts
    INTERRUPT_GlobalInterruptDisable();
    
    if (Transition_to_Ground)
    {
        if (voltage <= 20)
        {
            OFF_500V_M_SetLow();
            OFF_500V_P_SetLow();
            
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
        if (voltage <= 20)
        {
            OFF_500V_P_SetHigh();
            OFF_500V_M_SetHigh();
            
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
        if (voltage <= 20)
        {
            OFF_500V_M_SetHigh(); 
            OFF_500V_P_SetLow();
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
        if (voltage <= 20)
        {
            OFF_500V_P_SetHigh(); 
            OFF_500V_M_SetLow();
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
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
}
