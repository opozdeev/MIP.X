#ifndef MEASURE_H
#define	MEASURE_H

#include "high_voltage_control.h"
#include "mcc_generated_files/mcc.h"

typedef struct
{
    float voltage;
    float current;
    float resistance;
} measures;

#define MAX_VAL_ADC     65536.0
#define U_ref           4.096
#define R_div_fb        9900000.0
#define R_U_fb          56000.0
#define R_I_fb          5600.0

#define NUM_OF_SAMPLES  15
#define MEDIAN          7

adc_result_t comp (const adc_result_t *, const adc_result_t *);
void bubble_sort(adc_result_t list[], int8_t n);

measures get_measure(void);


#endif	/* MEASURE_H */

