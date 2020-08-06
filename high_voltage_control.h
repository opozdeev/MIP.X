#ifndef HIGH_VOLTAGE_CONTROL_H
#define	HIGH_VOLTAGE_CONTROL_H

#include "mcc_generated_files/epwm1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/interrupt_manager.h"
#include <stdbool.h>

extern union uIndData Indik;

void TurnON_500V_Minus(void);

void TurnON_500V_Plus(void);

void TurnOFF_500V(void);

void TurnON_GND(void);

void safe_switch(float voltage);

#endif	/* HIGH_VOLTAGE_CONTROL_H */

