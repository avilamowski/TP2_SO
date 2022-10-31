#include <lib.h>
#include <speaker.h>
#include <time.h>
void playSound(uint64_t frequency, uint64_t ticks){
    if(frequency > 0){
        startSound(frequency);
    }
    int initialTicks = ticksElapsed();
    while(ticksElapsed() - initialTicks < ticks){
        _hlt();
    }
    if(frequency > 0)
        stopSound();
}