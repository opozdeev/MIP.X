#include "measure.h"

measures get_measure(void)
{
    static bool update_calibr_coef = 0;
    const float coef_U = (U_ref/MAX_VAL_ADC) * ((R_div_fb + R_U_fb) / R_U_fb);     
    const float coef_I = ((U_ref/MAX_VAL_ADC) / R_I_fb) * 1000000.0;
    const float coef_Uin = (U_ref/(MAX_VAL_ADC * UIN_div_amp)) * ((UIN_div_high + UIN_div_low) / UIN_div_low);//0.015326439950980392156862745098039

    float U_coef_calibr, U_bias_calibr, I_above_100_coef_calibr, I_above_100_bias_calibr, I_below_100_coef_calibr, R_bias_calibr, Uin_coef_calibr, Uin_bias_calibr;
    //eeprom_write_object(0x01, &coef_U_calibr, sizeof(coef_U_calibr));
    
    eeprom_read_object(0x10, &U_coef_calibr, sizeof(float));
    eeprom_read_object(0x14, &U_bias_calibr, sizeof(float));
    eeprom_read_object(0x18, &I_above_100_coef_calibr, sizeof(float));
    eeprom_read_object(0x1C, &I_above_100_bias_calibr, sizeof(float));
    eeprom_read_object(0x20, &I_below_100_coef_calibr, sizeof(float));
    eeprom_read_object(0x0C, &R_bias_calibr, sizeof(float));
    
    uint32_t i, j;
    uint32_t sum_adres_U = 0, sum_adres_I = 0, sum_adres_Uin = 0;
    adc_result_t I_array[NUM_OF_SAMPLES];
    adc_result_t U_array[NUM_OF_SAMPLES];
    adc_result_t Uin_array[NUM_OF_SAMPLES];
    
    measures measure = {0, 0, 0, 0};
    
    if (!update_calibr_coef)
    {
        
        update_calibr_coef = true;
    }
 /*
    for (j = 0; j < MEAN_NUM; j++)
    {
        for (i = 0; i < NUM_OF_SAMPLES; i++)
        {
            U_array[i] = ADC_GetConversion(FB_U);            
        }
        __delay_us(10);
        for (i = 0; i < NUM_OF_SAMPLES; i++)
        {
            I_array[i] = ADC_GetConversion(FB_I);
        }
        __delay_us(10);
        for (i = 0; i < NUM_OF_SAMPLES; i++)
        {
            Uin_array[i] = ADC_GetConversion(Vin);
        }
       
        I_array[MEDIAN] = mean(I_array, NUM_OF_SAMPLES);//bubble_sort(I_array, NUM_OF_SAMPLES);//qsort(I_array, NUM_OF_SAMPLES, sizeof(adc_result_t), (adc_result_t(*) (const void *, const void *)) comp);
        U_array[MEDIAN] =  mean(U_array, NUM_OF_SAMPLES);//bubble_sort(U_array, NUM_OF_SAMPLES);//qsort(U_array, NUM_OF_SAMPLES, sizeof(adc_result_t), (adc_result_t(*) (const void *, const void *)) comp);
        Uin_array[MEDIAN] =  mean(Uin_array, NUM_OF_SAMPLES);
        

        sum_adres_I += I_array[MEDIAN];
        sum_adres_U += U_array[MEDIAN];
        sum_adres_Uin += Uin_array[MEDIAN];
     
    }
 */
    sum_adres_I = GetSampleMean(2);
    sum_adres_U = GetSampleMean(1);
    sum_adres_Uin = GetSampleMean(0);
    
    measure.voltage = U_coef_calibr * (coef_U * sum_adres_U/* / MEAN_NUM*/ - U_bias_calibr);
    
    measure.current = (sum_adres_I/* / MEAN_NUM*/) * coef_I;
    
/*    if (measure.current < (100 * measure.voltage / 500.0))
    {
        measure.current = measure.current * I_below_100_coef_calibr;
    }
    else
    {*/
        measure.current = I_above_100_coef_calibr * (measure.current - I_above_100_bias_calibr);
/*    }*/
    /*
    if (measure.voltage < 0)
    {
        measure.voltage = 0;
    }
    
    if (measure.current < 0)
    {
        measure.current = 0;
    }
    */    
    measure.voltagein = /*Uin_coef_calibr * */(coef_Uin * (sum_adres_Uin/* / MEAN_NUM*/ - MAX_VAL_ADC / 2) - I_below_100_coef_calibr/* - Uin_bias_calibr*/);
/*
    if (measure.voltagein < 0)
    {
        measure.voltagein = 1;
    }
*/    
    if (measure.voltage > 50)//проверим что напряжение ИОНа достаточно для измерения сопротивления
    {
        measure.resistance = (measure.voltage / measure.current) - R_bias_calibr;

//        if (measure.resistance < 0) measure.resistance = 0;
    }
    else 
    {
        unsigned long tmp = 0xff800000;
        measure.resistance = (float) tmp;//отправим -inf
    }
        
    return measure;
}
/* функция для сортировки отсчётов */
adc_result_t comp (const adc_result_t *i, const adc_result_t *j)
{
    return *i - *j;
}
/* функция сортировки отсчётов */
void bubble_sort(adc_result_t list[], int8_t n)
{
  int8_t c, d;
  adc_result_t t;
 
  for (c = 0 ; c < n - 1; c++)
  {
    for (d = 0 ; d < n - c - 1; d++)
    {
      if (list[d] > list[d+1])
      {
        /* Swapping */
 
        t         = list[d];
        list[d]   = list[d+1];
        list[d+1] = t;
      }
    }
  }
}

unsigned short mean(unsigned short* Data, unsigned int Size)
{
    unsigned int i;
    unsigned long Sum = 0;
    if (!Size) return 0;
    for (i=0; i < Size; i++)
        Sum += Data[i];
    return Sum/Size;
}

static adc_result_t SampleMean[3] = {0,0,0};

#define NUM_SAMPLES 256

void AddSample(adc_result_t Sample, unsigned char Ch)
{
 static unsigned short SampleCount[3] = {0,0,0};
 static unsigned long Sum[3] = {0,0,0};
    switch (Ch)
    {
        case Vin:  
            
            Sum[0] += Sample;
            SampleCount[0]++;
            if (SampleCount[0] > NUM_SAMPLES)
            {
                SampleMean[0] = Sum[0] /NUM_SAMPLES;
                SampleCount[0] = 0;
                Sum[0] = 0;
            }
            break;
        case FB_U: 

            Sum[1] += Sample;
            SampleCount[1]++;
            if (SampleCount[1] > NUM_SAMPLES)
            {
//            LATCbits.LATC6 = 1;//проверка частоты срабатывания                
                SampleMean[1] = Sum[1] /NUM_SAMPLES;
                SampleCount[1] = 0;
                Sum[1] = 0;
//            LATCbits.LATC6 = 0;//проверка частоты срабатывания                
            }
            break;
        case FB_I:   
            
            Sum[2] += Sample;
            SampleCount[2]++;
            if (SampleCount[2] > NUM_SAMPLES)
            {
                SampleMean[2] = Sum[2] /NUM_SAMPLES;
                SampleCount[2] = 0;
                Sum[2] = 0;
            }
            break;
    }
}

adc_result_t GetSampleMean(unsigned char Ch)
{
    return SampleMean[Ch];
}