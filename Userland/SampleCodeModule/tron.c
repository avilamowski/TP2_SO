#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>
#include <color.h>

#define SPACEBAR 0x39
#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_ENTER 0x1C
#define KEY_UP 0x48
#define KEY_LEFT 0x4B
#define KEY_DOWN 0x50
#define KEY_RIGHT 0x4D

#define PLAYER_SIZE 10
#define DELTA_TICK 1
#define SPEED 10

#define FIELD_WIDTH (1024/10)
#define FIELD_HEIGHT (768/10)

typedef struct {
    uint16_t x, y;
    uint16_t vx, vy;
    Color color, trailColor;
} Player;

static void loop();
static void checkCollision(Player * p);
static void update(Player * p);
static void initializeField();

Player _p1, _p2;
uint8_t _playing;
//uint16_t _width, _height;
char _field[FIELD_WIDTH][FIELD_HEIGHT];

void startTron() {
    //uint32_t res = getScreenResolution();
    //_width = res & 0x0000FFFF;
    //_height = res >> 16; 

    initializeField();
    clear();
    puts("Bienvenido a tron\n");
    puts("Instrucciones: bla bla bla\n");
    puts("Presione espacio para comenzar\n");
    _playing = 1;

    uint64_t initialTicks = getTicks();
    _p1 = (Player){ FIELD_WIDTH * PLAYER_SIZE * 0.25, FIELD_HEIGHT*PLAYER_SIZE/2, SPEED,  0, (Color){255,153,184}, (Color){255,0,184}};
    _p2 = (Player){ FIELD_WIDTH * PLAYER_SIZE * 0.75, FIELD_HEIGHT*PLAYER_SIZE/2, -SPEED, 0, (Color){100,255,100}, (Color){100,150,100}};
    uint64_t ticks = getTicks();
    uint64_t nextTicks;
    while (_playing) {
        nextTicks = getTicks();
        if (nextTicks - ticks > DELTA_TICK) {
            ticks = nextTicks;
            loop(_field);
        }
    }
    //clear();
    puts("Fin del Juego hm?!!!!");
}

static void initializeField() {
    for (int i = 0; i < FIELD_WIDTH; i++)
        for (int j = 0; j < FIELD_HEIGHT; j++)
            _field[i][j] = 0;
    
    for(int i = 0; i < FIELD_WIDTH; i++){
        _field[i][0] = 1;
        _field[i][FIELD_HEIGHT-1] = 1;
    }

    for(int j = 0; j < FIELD_HEIGHT; j++){
        _field[0][j] = 1;
        _field[FIELD_WIDTH - 1][j] = 1;
    }

}

static void loop() {
    drawRect(_p1.x, _p1.y, PLAYER_SIZE, PLAYER_SIZE, _p1.trailColor);
    drawRect(_p2.x, _p2.y, PLAYER_SIZE, PLAYER_SIZE, _p2.trailColor);

    switch (getScanCode()) {
        case KEY_UP:
            _p1.vx = 0;     
            _p1.vy = -1*SPEED;     
            break;
        case KEY_LEFT:
            _p1.vx = -1*SPEED;
            _p1.vy = 0;
            break;
        case KEY_DOWN:
            _p1.vx = 0;
            _p1.vy = 1*SPEED;
            break;
        case KEY_RIGHT:
            _p1.vx = 1*SPEED;
            _p1.vy = 0;
            break;
        case KEY_W:
            _p2.vx = 0;     
            _p2.vy = -1*SPEED;     
            break;
        case KEY_A:
            _p2.vx = -1*SPEED;
            _p2.vy = 0;
            break;
        case KEY_S:
            _p2.vx = 0;
            _p2.vy = 1*SPEED;
            break;
        case KEY_D:
            _p2.vx = 1*SPEED;
            _p2.vy = 0;
            break;
        default:
            break;
    }
    update(&_p1);
    update(&_p2);
}

static void checkCollision(Player * p){
    if(_field[p->x/PLAYER_SIZE][p->y/PLAYER_SIZE]){
        _playing = 0;
    }
}

static void update(Player * p) {
    p->x += p->vx;
    p->y += p->vy;
    checkCollision(p);
    _field[p->x/PLAYER_SIZE][p->y/PLAYER_SIZE] = 1;
    drawRect(p->x, p->y, PLAYER_SIZE, PLAYER_SIZE, p->color);
}