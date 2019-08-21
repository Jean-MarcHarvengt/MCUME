#include <stdint.h>
#include <string.h>
#include <Arduino.h>

#ifdef MEGA
  #include <TimerOne.h>
#else
IntervalTimer myTimer;
#endif

#define PIT_MODE_LATCHCOUNT  0
#define PIT_MODE_LOBYTE 1
#define PIT_MODE_HIBYTE 2
#define PIT_MODE_TOGGLE 3


struct i8253_s {
  uint16_t chandata[3];
  uint8_t accessmode[3];
  uint8_t bytetoggle[3];
  uint32_t effectivedata[3];
  float chanfreq[3];
  uint8_t active[3];
  uint16_t counter[3];
} i8253;

volatile uint8_t timerTick = 0;

void timer_isr() {
  timerTick = 1;
}

void out8253 (uint16_t portnum, uint8_t value) {
  uint8_t curbyte;
  portnum &= 3;
  switch (portnum) {
      case 0:
      case 1:
      case 2: //channel data
        if ( (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if (curbyte == 0) { //low byte
            i8253.chandata[portnum] = (i8253.chandata[portnum] & 0xFF00) | value;
          }
        else {   //high byte
            i8253.chandata[portnum] = (i8253.chandata[portnum] & 0x00FF) | ( (uint16_t) value << 8);
          }
        if (i8253.chandata[portnum] == 0) i8253.effectivedata[portnum] = 65536;
        else i8253.effectivedata[portnum] = i8253.chandata[portnum];
        i8253.active[portnum] = 1;
        if (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) i8253.bytetoggle[portnum] = (~i8253.bytetoggle[portnum]) & 1;
        //i8253.chanfreq[portnum] = (float) ( (uint32_t) ( ( (float) 1193182.0 / (float) i8253.effectivedata[portnum]) * (float) 1000.0) );
        //Serial.print("period "); Serial.println((uint32_t) ((float)1000000.0 / ( ( (float) 1193182.0 / (float) i8253.effectivedata[portnum]))));
        if (portnum == 0) {
          uint32_t period;
          period = (uint32_t) ((float)1000000.0 / ( ( (float) 1193182.0 / (float) i8253.effectivedata[portnum])));
#ifdef MEGA
          if (period < 4000) period = 4000; //limit to 250 Hz, or the emulator just can't keep up on a Mega
          //Serial.println((float)1000000.0 / (float)period);
          Timer1.attachInterrupt(timer_isr, period);
#else
          myTimer.begin(timer_isr, period);
#endif
        }
        break;
      case 3: //mode/command
        i8253.accessmode[value>>6] = (value >> 4) & 3;
        if (i8253.accessmode[value>>6] == PIT_MODE_TOGGLE) i8253.bytetoggle[value>>6] = 0;
        break;
    }
}

uint8_t in8253 (uint16_t portnum) {
  uint8_t curbyte;
  portnum &= 3;
  switch (portnum) {
      case 0:
      case 1:
      case 2: //channel data
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) ) i8253.bytetoggle[portnum] = (~i8253.bytetoggle[portnum]) & 1;
        if (curbyte == 0) { //low byte
          if (i8253.counter[portnum] < 10) i8253.counter[portnum] = i8253.chandata[portnum];
          i8253.counter[portnum] -= 10;
            return ( (uint8_t) i8253.counter[portnum]);
          }
        else {   //high byte
            return ( (uint8_t) (i8253.counter[portnum] >> 8) );
          }
        break;
    }
  return (0);
}

void init8253() {
  memset (&i8253, 0, sizeof (i8253) );
#ifdef MEGA
  Timer1.initialize(54925);
  Timer1.attachInterrupt(timer_isr, 54925);
#else
  myTimer.begin(timer_isr, 54925);
#endif
  //set_port_write_redirector (0x40, 0x43, &out8253);
  //set_port_read_redirector (0x40, 0x43, &in8253);
}

