#include "measure.h"
#include <math.h>

measures get_measure(void)
{
    static bool update_calibr_coef = 0;
    const float coef_U = (U_ref/MAX_VAL_ADC) * ((R_div_fb + R_U_fb) / R_U_fb);     
    const float coef_I = ((U_ref/MAX_VAL_ADC) / R_I_fb) * 1000000.0;
    const float coef_Uin = (U_ref/(MAX_VAL_ADC * UIN_div_amp)) * ((UIN_div_high + UIN_div_low) / UIN_div_low);//0.015326439950980392156862745098039

    float U_coef_calibr, U_bias_calibr, I_above_100_coef_calibr, I_above_100_bias_calibr, I_below_100_coef_calibr, R_coef_calibr, R_bias_calibr, Uin_coef_calibr, Uin_bias_calibr;
    //eeprom_write_object(0x01, &coef_U_calibr, sizeof(coef_U_calibr));
/*    
    eeprom_read_object(0x10, &U_coef_calibr, sizeof(float));
    eeprom_read_object(0x14, &U_bias_calibr, sizeof(float));
    eeprom_read_object(0x18, &I_above_100_coef_calibr, sizeof(float));
    eeprom_read_object(0x1C, &I_above_100_bias_calibr, sizeof(float));
    eeprom_read_object(0x20, &I_below_100_coef_calibr, sizeof(float));
    eeprom_read_object(0x0C, &R_bias_calibr, sizeof(float));
*/

    eeprom_read_object2(0x00, &Uin_coef_calibr, sizeof(float));//Uin_coef_calibr
    eeprom_read_object2(0x04, &Uin_bias_calibr, sizeof(float));//Uin_bias_calibr
    eeprom_read_object2(0x08, &R_coef_calibr, sizeof(float));//R_coef_calibr = 1
    eeprom_read_object2(0x0c, &R_bias_calibr, sizeof(float));//R_bias_calibr

    eeprom_read_object2(0x10, &U_coef_calibr, sizeof(float));//U_coef_calibr
    eeprom_read_object2(0x14, &U_bias_calibr, sizeof(float));//U_bias_calibr
    eeprom_read_object2(0x18, &I_above_100_coef_calibr, sizeof(float));//I_above_100_coef_calibr
    eeprom_read_object2(0x1c, &I_above_100_bias_calibr, sizeof(float));//I_above_100_bias_calibr
    eeprom_read_object2(0x20, &I_below_100_coef_calibr, sizeof(float));//I_below_100_coef_calibr
    
    measures measure = {0, 0, 0, 0};
    
    if (!update_calibr_coef)
        update_calibr_coef = true;
     
    unsigned short TmpValue = GetSampleMean(FB_U);
    measure.voltage = U_coef_calibr * (coef_U * TmpValue/*GetSampleMean(FB_U)*/ - U_bias_calibr);
    
    TmpValue = GetSampleMean(FB_I);
    measure.current = TmpValue /*GetSampleMean(FB_I)*/ * coef_I;
    
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
//    measure.voltagein = Uin_coef_calibr * (coef_Uin * GetSampleMean(Vin) - Uin_bias_calibr);//измерение постоянки
    measure.voltagein = Uin_coef_calibr * (coef_Uin * GetSampleRms() - Uin_bias_calibr);//измерение переменки (средняя сумма квадратов)
    
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

static /*adc_result_t*/short SampleMean[3] = {0,0,0};
static float SampleRMS = 0;

#define NUM_SAMPLES 4166//2048//256//число отсчётов должно быть кратно периоду измеряемого сигнала

void AddSample(adc_result_t Sample, unsigned char Ch)
{
 static unsigned short SampleCount[3] = {0,0,0};
 static long SumMean[3] = {0,0,0};
 static unsigned long long SumPower2 = 0;
 static short TmpRes;
    switch (Ch)
    {
        case Vin:  
            /* Среднее
            SumMean[0] += Sample;
            SampleCount[0]++;
            if (SampleCount[0] >= NUM_SAMPLES)
            {
                SampleMean[0] = SumMean[0] /NUM_SAMPLES;
                SampleCount[0] = 0;
                SumMean[0] = 0;
            }
            */
            TmpRes = (short)Sample - 65472/2;
            SumPower2 +=  TmpRes *  TmpRes;
            SumMean[0] += TmpRes;
            SampleCount[0]++;
            if (SampleCount[0] >= NUM_SAMPLES)
            {
                SampleRMS = sqrtf(SumPower2 / NUM_SAMPLES);
                SampleMean[0] = SumMean[0] /NUM_SAMPLES;
                SampleCount[0] = 0;
                SumPower2 = 0;
                SumMean[0] = 0;
            }
            break;
        case FB_U: 

            SumMean[1] += Sample;
            SampleCount[1]++;
            if (SampleCount[1] >= NUM_SAMPLES)
            {
//            LATCbits.LATC6 = 1;//проверка частоты срабатывания                
                SampleMean[1] = SumMean[1] /NUM_SAMPLES;
                SampleCount[1] = 0;
                SumMean[1] = 0;
//            LATCbits.LATC6 = 0;//проверка частоты срабатывания                
            }
            break;
        case FB_I:   
            
            SumMean[2] += Sample;
            SampleCount[2]++;
            if (SampleCount[2] >= NUM_SAMPLES)
            {
                SampleMean[2] = SumMean[2] /NUM_SAMPLES;
                SampleCount[2] = 0;
                SumMean[2] = 0;
            }
            break;
    }
}

short GetSampleMean(unsigned char Ch)
{
    switch (Ch)
    {
        case Vin:
            return SampleMean[0];
        case FB_U:
            return SampleMean[1];
        case FB_I:
            return SampleMean[2];
    }
}
float GetSampleRms()
{
    return SampleRMS;
}