#ifndef MEASURE_H
#define	MEASURE_H

#include "high_voltage_control.h"
#include "mcc_generated_files/mcc.h"

typedef struct
{
    float voltage;
    float current;
    float resistance;
    float voltagein;
} measures;

#define MAX_VAL_ADC     65536.0//максимальное значение с АЦП
#define U_ref           4.096//напряжение референса
#define R_div_fb        9900000.0//больший резистор в делителе высокого напряжения 
#define R_U_fb          100000.0//56000.0//меньший резистор в делителе высокого напряжения
#define R_I_fb          22000.0//5600.0//токовый шунт ИОНа
#define R_LIM           1000000.0//ограничительное сопротивление на выходе ИОНа
#define UIN_div_high    100000.0//верхний резистор в делителе напряжения
#define UIN_div_low     24.0//нижний резистор в делителе напряжения
#define UIN_div_amp     8.0//коэффициент усиления изолирующего усилителя

#define NUM_OF_SAMPLES  15
#define MEDIAN          7

adc_result_t comp (const adc_result_t *, const adc_result_t *);
void bubble_sort(adc_result_t list[], int8_t n);

measures get_measure(void);
unsigned short mean(unsigned short* Data, unsigned int Size);


#endif	/* MEASURE_H */

