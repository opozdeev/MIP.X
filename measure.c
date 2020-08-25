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
   
    
 /*unsigned short*/float TmpValue;
    
    if (IsResultReady(2))
    {
        TmpValue = GetResult(2) / ((float)NUM_SAMPLES);//GetSampleMean(FB_U);
        measure.voltage = U_coef_calibr * (coef_U * TmpValue - U_bias_calibr);
/*
    if (measure.voltage < 0)
    {
        measure.voltage = 0;
    }
*/
    }
    
    if (IsResultReady(3))
    {
    TmpValue = GetResult(3) / ((float)NUM_SAMPLES);//GetSampleMean(FB_I);
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
    
    if (measure.current < 0)
    {
        measure.current = 0;
    }
    */    
    }
 float VoltageInDc;
 short VoltageInDcInt;

        if (IsResultReady(1))//проверяем готовность данных по каналу DC напряжения полюса
        {
            /*measure.voltagein*/VoltageInDc = GetResult(1) / ((float)NUM_SAMPLES);// /*Uin_coef_calibr * (coef_Uin * */GetSampleMean(Vin)/* - Uin_bias_calibr)*/;//измерение постоянки
            VoltageInDcInt = VoltageInDc;
        }
    
        if (IsResultReady(0))//проверяем готовность данных по каналу RMS
        {
            measure.voltagein = Uin_coef_calibr * (coef_Uin * sqrtf(GetResult(0) / ((float)NUM_SAMPLES) - VoltageInDc * VoltageInDc) - Uin_bias_calibr);//измерение переменки (средняя сумма квадратов)
        }
    
/*
    if (measure.voltagein < 0)
    {
        measure.voltagein = 1;
    }
*/    
    if (measure.voltage > 50)//проверим что напряжение ИОНа достаточно для измерения сопротивления
    {
//на время тестирования        measure.resistance = (measure.voltage / measure.current) - R_bias_calibr;

//        if (measure.resistance < 0) measure.resistance = 0;
    }
    else 
    {
        unsigned long tmp = 0xff800000;
//на время тестирования        measure.resistance = (float) tmp;//отправим -inf
    }
measure.resistance = VoltageInDc;//на время тестирования        
    return measure;
}
//---------------------------------------------------------------------------------------------
/* функция для сортировки отсчётов */
adc_result_t comp (const adc_result_t *i, const adc_result_t *j)
{
    return *i - *j;
}
//---------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------
unsigned short mean(unsigned short* Data, unsigned int Size)
{
    unsigned int i;
    unsigned long Sum = 0;
    if (!Size) return 0;
    for (i=0; i < Size; i++)
        Sum += Data[i];
    return Sum/Size;
}
//---------------------------------------------------------------------------------------------
static /*adc_result_t*/short SampleMean[3] = {0,0,0};
static float SampleRMS = 0;

//#define NUM_SAMPLES (1565/3)//4166//2048//256//число отсчётов должно быть кратно периоду измеряемого сигнала

void AddSample(adc_result_t Sample, unsigned char Ch)
{
 static unsigned short SampleCount[3] = {0,0,0};
 static long SumMean[3] = {0,0,0};
 static long SumPower2 = 0;//32-bit
 static /*short*/ long TmpRes;
 
    switch (Ch)
    {
        case Vin:  
//            LATBbits.LATB6 = ~LATBbits.LATB6;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            TmpRes = (short)(Sample) - (0x3FF/2);//убираем смещение (ref/2)
            SumPower2 += TmpRes * TmpRes;//10-bit * 10-bit = 20-bit => 64-bit //накопление для RMS
            SumMean[0] += TmpRes;//накопление для среднего
            SampleCount[0]++;
            if (SampleCount[0] >= NUM_SAMPLES)
            {
                SaveResult(SumPower2, 0);//переписать во временное хранилище для дальнейшего расчёта//SampleMean[0] = SumMean[0] /NUM_SAMPLES;
                SaveResult(SumMean[0], 1);//переписать во временное хранилище для дальнейшего расчёта//TmpRMS = (long)(SumPower2 / NUM_SAMPLES) - SampleMean[0] * SampleMean[0];
                //sqrtf(TmpRMS);
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
                SaveResult(SumMean[1], 2);//SampleMean[1] = SumMean[1] /NUM_SAMPLES;
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
                SaveResult(SumMean[2], 3);//SampleMean[2] = SumMean[2] /NUM_SAMPLES;
                SampleCount[2] = 0;
                SumMean[2] = 0;
            }
            break;
    }
}
//---------------------------------------------------------------------------------------------
short GetSampleMean(unsigned char Ch)
{
    short Tmp = 0;
    switch (Ch)
    {
        case Vin:
            INTERRUPT_GlobalInterruptLowDisable();
            Tmp = SampleMean[0];
            INTERRUPT_GlobalInterruptLowEnable();
            break;
        case FB_U:
            INTERRUPT_GlobalInterruptLowDisable();
            Tmp = SampleMean[1];
            INTERRUPT_GlobalInterruptLowEnable();
            break;
        case FB_I:
            INTERRUPT_GlobalInterruptLowDisable();
            Tmp = SampleMean[2];
            INTERRUPT_GlobalInterruptLowEnable();
            break;
    }
    return Tmp;
}
//---------------------------------------------------------------------------------------------
float GetSampleRms()
{
    float Tmp;
    INTERRUPT_GlobalInterruptLowDisable();
    Tmp = SampleRMS;
    INTERRUPT_GlobalInterruptLowEnable();
    return Tmp;
}
//---------------------------------------------------------------------------------------------
static long MesArray[4] = {0,0,0,0};//в массиве будем хранить данные для дальнейших вычислений
static bool ValueUsed[4] = {true, true, true, true};
/*
 * Сохраним результат измерений
 * Если если предыдущее сохранение было считано - возвращаем false (пока непонятный функционал)
 */
bool SaveResult(long Value, unsigned char Pos)
{
    MesArray[Pos] = Value;
    if (ValueUsed[Pos])
    {
        ValueUsed[Pos] = false;
        return true;
    }
    else return false;
}
//---------------------------------------------------------------------------------------------
/*
 возвращает результат измерений и устанавливает флаг использованности результата измерений
 */
long GetResult(unsigned char Pos)
{
    ValueUsed[Pos] = true;
    return MesArray[Pos];
}
//---------------------------------------------------------------------------------------------
/* 
 если результат измерений готов - возвращает true 
 */
bool IsResultReady(unsigned char Pos)
{
    return ~ValueUsed[Pos];
}