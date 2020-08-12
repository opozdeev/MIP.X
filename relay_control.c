#include "relay_control.h"

bool relay_coil_state = false;

void switch_relay_coil( bool new_state ){
    relay_coil_state = new_state;
    
    // action to force coil
    // ...
}