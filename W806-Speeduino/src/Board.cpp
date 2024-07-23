
#include "board.h"

ScheduleCounter EIOSCounter;
extern void fuelSchedule1Interrupt();
extern void fuelSchedule2Interrupt();
extern void fuelSchedule3Interrupt();
extern void fuelSchedule4Interrupt();
extern void fuelSchedule5Interrupt();
extern void fuelSchedule6Interrupt();
extern void fuelSchedule7Interrupt();
extern void fuelSchedule8Interrupt();
extern void ignitionSchedule1Interrupt();
extern void ignitionSchedule2Interrupt();
extern void ignitionSchedule3Interrupt();
extern void ignitionSchedule4Interrupt();
extern void ignitionSchedule5Interrupt();
extern void ignitionSchedule6Interrupt();
extern void ignitionSchedule7Interrupt();
extern void ignitionSchedule8Interrupt();

ScheduleCompare Fuel1Schedule([]{fuelSchedule1Interrupt();});
ScheduleCompare Fuel2Schedule([]{fuelSchedule2Interrupt();});
ScheduleCompare Fuel3Schedule([]{fuelSchedule3Interrupt();});
ScheduleCompare Fuel4Schedule([]{fuelSchedule4Interrupt();});
ScheduleCompare Fuel5Schedule([]{fuelSchedule5Interrupt();});
ScheduleCompare Fuel6Schedule([]{fuelSchedule6Interrupt();});
ScheduleCompare Fuel7Schedule([]{fuelSchedule7Interrupt();});
ScheduleCompare Fuel8Schedule([]{fuelSchedule8Interrupt();});
ScheduleCompare Ignition1Schedule([]{ignitionSchedule1Interrupt();});
ScheduleCompare Ignition2Schedule([]{ignitionSchedule2Interrupt();});
ScheduleCompare Ignition3Schedule([]{ignitionSchedule3Interrupt();});
ScheduleCompare Ignition4Schedule([]{ignitionSchedule4Interrupt();});
ScheduleCompare Ignition5Schedule([]{ignitionSchedule5Interrupt();});
ScheduleCompare Ignition6Schedule([]{ignitionSchedule6Interrupt();});
ScheduleCompare Ignition7Schedule([]{ignitionSchedule7Interrupt();});
ScheduleCompare Ignition8Schedule([]{ignitionSchedule8Interrupt();});

extern void boostInterrupt();
extern void vvtInterrupt();

ScheduleCompare BoostSchedule([]{boostInterrupt();});
ScheduleCompare VVTSchedule([]{vvtInterrupt();});

extern void idleInterrupt();

ScheduleCompare IdleSchedule([]{idleInterrupt();});

void initBoard(void)
{
    
}
uint16_t freeRam(void)
{
    uint32_t freeRam;
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t)&stackTop;

    // current position of heap.
    void *hTop = malloc(1);
    heapTop = (uint32_t)hTop;
    free(hTop);
    freeRam = stackTop - heapTop;

    if(freeRam>0xFFFF){return 0xFFFF;}
    else{return freeRam;}
}
void doSystemReset(void)
{

}
void jumpToBootloader(void)
{
    
}