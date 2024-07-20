#include "ITimerService.h"

#ifndef BOARD_W806_H
#define BOARD_W806_H

/*
***********************************************************************************************************
* General
*/
  #define INJ_CHANNELS 8
  #define IGN_CHANNELS 8
  #define BOARD_MAX_IO_PINS 100
  #define BOARD_MAX_DIGITAL_PINS 100
  #define PORT_TYPE uint32_t
  #define PINMASK_TYPE uint32_t
  #define RTC_LIB_H "stdlib.h"

  #define COMPARE_TYPE EmbeddedIOServices::tick_t
  #define COUNTER_TYPE EmbeddedIOServices::tick_t
  #define SERIAL_BUFFER_SIZE (256+7+1) //Size of the serial buffer used by new comms protocol. The largest single packet is the O2 calibration which is 256 bytes + 7 bytes of overhead
  #define FPU_MAX_SIZE 0 //Size of the FPU buffer. 0 means no FPU.
  #define EEPROM_LIB_H "W806EEPROM.h"
  typedef int eeprom_address_t;
  void initBoard(void);
  uint16_t freeRam(void);
  void doSystemReset(void);
  void jumpToBootloader(void);
  #define micros_safe() micros()
  #define pinIsReserved(pin)  ( ((pin) == 0) ) //Forbidden pins like USB on other boards

  #define BLOCKING_FACTOR       121
  #define TABLE_BLOCKING_FACTOR 64
  #define SECONDARY_SERIAL_T Stream

/*
***********************************************************************************************************
* Schedules
*/

  extern EmbeddedIOServices::ITimerService *Speeduino_TimerService;
  struct ScheduleCounter
  {
    EmbeddedIOServices::tick_t operator+(unsigned int a)
    {
      return Speeduino_TimerService->GetTick() + a;
    }
  };

  extern ScheduleCounter EIOSCounter;

  struct ScheduleCompare 
  {
    EmbeddedIOServices::Task task;
    ScheduleCompare(EmbeddedIOServices::callback_t callBack) : task(callBack)
    {

    }

    void operator=(EmbeddedIOServices::tick_t a)
    {
      Speeduino_TimerService->ScheduleTask(&task, a);
    }
  };
  
  extern ScheduleCompare Fuel1Schedule;
  extern ScheduleCompare Fuel2Schedule;
  extern ScheduleCompare Fuel3Schedule;
  extern ScheduleCompare Fuel4Schedule;
  extern ScheduleCompare Fuel5Schedule;
  extern ScheduleCompare Fuel6Schedule;
  extern ScheduleCompare Fuel7Schedule;
  extern ScheduleCompare Fuel8Schedule;
  extern ScheduleCompare Ignition1Schedule;
  extern ScheduleCompare Ignition2Schedule;
  extern ScheduleCompare Ignition3Schedule;
  extern ScheduleCompare Ignition4Schedule;
  extern ScheduleCompare Ignition5Schedule;
  extern ScheduleCompare Ignition6Schedule;
  extern ScheduleCompare Ignition7Schedule;
  extern ScheduleCompare Ignition8Schedule;

  #define FUEL1_COUNTER EIOSCounter
  #define FUEL2_COUNTER EIOSCounter
  #define FUEL3_COUNTER EIOSCounter
  #define FUEL4_COUNTER EIOSCounter
  #define FUEL5_COUNTER EIOSCounter
  #define FUEL6_COUNTER EIOSCounter
  #define FUEL7_COUNTER EIOSCounter
  #define FUEL8_COUNTER EIOSCounter

  #define IGN1_COUNTER  EIOSCounter
  #define IGN2_COUNTER  EIOSCounter
  #define IGN3_COUNTER  EIOSCounter
  #define IGN4_COUNTER  EIOSCounter
  #define IGN5_COUNTER  EIOSCounter
  #define IGN6_COUNTER  EIOSCounter
  #define IGN7_COUNTER  EIOSCounter
  #define IGN8_COUNTER  EIOSCounter

  #define FUEL1_COMPARE Fuel1Schedule
  #define FUEL2_COMPARE Fuel2Schedule
  #define FUEL3_COMPARE Fuel3Schedule
  #define FUEL4_COMPARE Fuel4Schedule
  #define FUEL5_COMPARE Fuel5Schedule
  #define FUEL6_COMPARE Fuel6Schedule
  #define FUEL7_COMPARE Fuel7Schedule
  #define FUEL8_COMPARE Fuel8Schedule

  #define IGN1_COMPARE  Ignition1Schedule
  #define IGN2_COMPARE  Ignition2Schedule
  #define IGN3_COMPARE  Ignition3Schedule
  #define IGN4_COMPARE  Ignition4Schedule
  #define IGN5_COMPARE  Ignition5Schedule
  #define IGN6_COMPARE  Ignition6Schedule
  #define IGN7_COMPARE  Ignition7Schedule
  #define IGN8_COMPARE  Ignition8Schedule

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

  extern ScheduleCompare BoostSchedule;
  #define BOOST_TIMER_COMPARE   BoostSchedule
  #define BOOST_TIMER_COUNTER   EIOSCounter
  extern ScheduleCompare VVTSchedule;
  #define VVT_TIMER_COMPARE     VVTSchedule
  #define VVT_TIMER_COUNTER     EIOSCounter

/*
***********************************************************************************************************
* Idle
*/
  extern ScheduleCompare IdleSchedule;
  #define IDLE_COUNTER EIOSCounter
  #define IDLE_COMPARE IdleSchedule

  #define IDLE_TIMER_ENABLE() 
  #define IDLE_TIMER_DISABLE() 

/*
***********************************************************************************************************
* CAN / Second serial
*/

#endif