/* 
 * File:   Indikator.h
 * Author: Oleg
 *
 * Created on July 28, 2020, 9:25 AM
 */

#ifndef INDIKATOR_H
#define	INDIKATOR_H

#ifdef	__cplusplus
extern "C" {
#endif

    enum HVOUTS {OUTPLUS = 0x2, OUTMINUS = 0x1, OUTOFF = 0};//состояния выходного напряжения
    enum RELAYOUTS {NO = 1, NC = 2};
    enum HL1STATES {HL1GREEN = 1, HL1RED = 2, HL1OFF = 0};
    enum HL2STATES {HL2OFF = 1, HL2ON = 0};
    
    struct sIndData{
        unsigned int HvOut:2;//выход высокого напряжения
        unsigned int Hl2:1;//одноцветный светодиод
        unsigned int Hl1:2;//двухцветный светодиод
        unsigned int RelayOut:2;//сухие контакты
        bool AddrLatch:1;//строб записи адреса
        
        
        
        
    };
    
    union uIndData{
        struct sIndData sData;
        unsigned char ucData;
    };
    
    bool WriteOuts(union uIndData Data);
    
    unsigned char get_addr(void);
    void set_addr(unsigned char Addr);
    
#ifdef	__cplusplus
}
#endif

#endif	/* INDIKATOR_H */

