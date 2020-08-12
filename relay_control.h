/* 
 * File:   relay_control.h
 * Author: boris
 *
 * Created on 12 августа 2020 г., 22:57
 */

#ifndef RELAY_CONTROL_H
#define	RELAY_CONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdbool.h>
/**
  @Summary
    Switcn relay coil.

  @Description
    This methon should use to switch on or switch off releay output

  @Param
    New state of relay coil: true - switch on, false - switch off

  @Example
    <code>
        switch_relay_coil( true );
    </code>
*/
void switch_relay_coil( bool new_state);


#ifdef	__cplusplus
}
#endif

#endif	/* RELAY_CONTROL_H */

