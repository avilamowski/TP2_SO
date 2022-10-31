#include <sound.h>
void playVictory1(){
    playSound(390,3);
    playSound(520,3);
    playSound(660,3);
    playSound(780,6);
    playSound(660,3);
    playSound(780,6);
}
void playDraw(){
    for(int i=0; i<18; i++)
        playSound(500-20*i,1);
}