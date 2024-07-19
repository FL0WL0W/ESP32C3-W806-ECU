#include "globals.h"

#ifndef BOARD_W806_H
#define BOARD_W806_H

/*
***********************************************************************************************************
* General
*/
  #define INJ_CHANNELS 8
  #define IGN_CHANNELS 8
  #define BOARD_MAX_IO_PINS 100
  #define PORT_TYPE uint32_t
  #define PINMASK_TYPE uint32_t

  #define COMPARE_TYPE uint16_t
  #define COUNTER_TYPE uint16_t
  #define SERIAL_BUFFER_SIZE (256+7+1) //Size of the serial buffer used by new comms protocol. The largest single packet is the O2 calibration which is 256 bytes + 7 bytes of overhead
  #define FPU_MAX_SIZE 0 //Size of the FPU buffer. 0 means no FPU.
  #define EEPROM_LIB_H <EEPROM.h>
  typedef int eeprom_address_t;
  void initBoard(void);
  uint16_t freeRam(void);
  void doSystemReset(void);
  void jumpToBootloader(void);
  #define micros_safe() micros()
  #define pinIsReserved(pin)  ( ((pin) == 0) ) //Forbidden pins like USB on other boards

  #define SECONDARY_SERIAL_T Stream

/*
***********************************************************************************************************
* Schedules
*/
  struct ScheduleCounter
  {

  };

  static ScheduleCounter EIOSCounter;

  struct ScheduleCompare 
  {

  };
  static ScheduleCompare Ignition1Schedule;
  static ScheduleCompare Fuel1Schedule;

  #define FUEL1_COUNTER EIOSCounter
  #define FUEL2_COUNTER 
  #define FUEL3_COUNTER 
  #define FUEL4_COUNTER 
  #define FUEL5_COUNTER 
  #define FUEL6_COUNTER 
  #define FUEL7_COUNTER 
  #define FUEL8_COUNTER 

  #define IGN1_COUNTER  EIOSCounter
  #define IGN2_COUNTER  
  #define IGN3_COUNTER  
  #define IGN4_COUNTER  
  #define IGN5_COUNTER  
  #define IGN6_COUNTER  
  #define IGN7_COUNTER  
  #define IGN8_COUNTER  

  #define FUEL1_COMPARE Fuel1Schedule
  #define FUEL2_COMPARE 
  #define FUEL3_COMPARE 
  #define FUEL4_COMPARE 
  #define FUEL5_COMPARE 
  #define FUEL6_COMPARE 
  #define FUEL7_COMPARE 
  #define FUEL8_COMPARE 

  #define IGN1_COMPARE  Ignition1Schedule
  #define IGN2_COMPARE  
  #define IGN3_COMPARE  
  #define IGN4_COMPARE  
  #define IGN5_COMPARE  
  #define IGN6_COMPARE  
  #define IGN7_COMPARE  
  #define IGN8_COMPARE  

  //Note that the interrupt flag is reset BEFORE the interrupt is enabled
static inline void FUEL1_TIMER_ENABLE(void) { ;} 
static inline void FUEL2_TIMER_ENABLE(void) {  } 
static inline void FUEL3_TIMER_ENABLE(void) {  } 
static inline void FUEL4_TIMER_ENABLE(void) {  } 
static inline void FUEL5_TIMER_ENABLE(void) {  } 
static inline void FUEL6_TIMER_ENABLE(void) {  } 
static inline void FUEL7_TIMER_ENABLE(void) {  } 
static inline void FUEL8_TIMER_ENABLE(void) {  } 

static inline void FUEL1_TIMER_DISABLE(void) {  } 
static inline void FUEL2_TIMER_DISABLE(void) {  } 
static inline void FUEL3_TIMER_DISABLE(void) {  } 
static inline void FUEL4_TIMER_DISABLE(void) {  } 
static inline void FUEL5_TIMER_DISABLE(void) {  } 
static inline void FUEL6_TIMER_DISABLE(void) {  } 
static inline void FUEL7_TIMER_DISABLE(void) {  } 
static inline void FUEL8_TIMER_DISABLE(void) {  } 

static inline void IGN1_TIMER_ENABLE(void) {  } 
static inline void IGN2_TIMER_ENABLE(void) {  } 
static inline void IGN3_TIMER_ENABLE(void) {  } 
static inline void IGN4_TIMER_ENABLE(void) {  } 
static inline void IGN5_TIMER_ENABLE(void) {  } 
static inline void IGN6_TIMER_ENABLE(void) {  } 
static inline void IGN7_TIMER_ENABLE(void) {  } 
static inline void IGN8_TIMER_ENABLE(void) {  } 

static inline void IGN1_TIMER_DISABLE(void) { } 
static inline void IGN2_TIMER_DISABLE(void) { } 
static inline void IGN3_TIMER_DISABLE(void) { } 
static inline void IGN4_TIMER_DISABLE(void) { } 
static inline void IGN5_TIMER_DISABLE(void) { } 
static inline void IGN6_TIMER_DISABLE(void) { } 
static inline void IGN7_TIMER_DISABLE(void) { } 
static inline void IGN8_TIMER_DISABLE(void) { } 

  #define MAX_TIMER_PERIOD 262140UL //The longest period of time (in uS) that the timer can permit (IN this case it is 65535 * 4, as each timer tick is 4uS)
  #define uS_TO_TIMER_COMPARE(uS1) ((uS1) >> 2) //Converts a given number of uS into the required number of timer ticks until that time has passed

/*
***********************************************************************************************************
* Auxiliaries
*/
  #define ENABLE_BOOST_TIMER()  
  #define DISABLE_BOOST_TIMER() 
  #define ENABLE_VVT_TIMER()    
  #define DISABLE_VVT_TIMER()   

  #define BOOST_TIMER_COMPARE   
  #define BOOST_TIMER_COUNTER   
  #define VVT_TIMER_COMPARE     
  #define VVT_TIMER_COUNTER     

/*
***********************************************************************************************************
* Idle
*/
  #define IDLE_COUNTER
  #define IDLE_COMPARE

  #define IDLE_TIMER_ENABLE() 
  #define IDLE_TIMER_DISABLE() 

/*
***********************************************************************************************************
* CAN / Second serial
*/

#endif