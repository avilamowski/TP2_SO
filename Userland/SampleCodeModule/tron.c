#include <stdint.h>
#include <syscalls.h>
#include <stdio.h>
#include <color.h>

#define SPACEBAR 0x39
#define KEY_W 0x11
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_Z 0x2C
#define KEY_ENTER 0x1C
#define KEY_UP 0x48
#define KEY_LEFT 0x4B
#define KEY_DOWN 0x50
#define KEY_RIGHT 0x4D
#define KEY_SLASH 0x35

#define PLAYER_SIZE 10
#define DELTA_TICK 1
#define SPEED 5
#define TURBO_TICKS 5
#define TURBO_COOLDOWN 10

#define FIELD_WIDTH (1024/PLAYER_SIZE) 
#define FIELD_HEIGHT (768/PLAYER_SIZE)
#define FIELD_WIDTH_POSITIONS (FIELD_WIDTH/8 + 1) 

typedef struct {
    uint16_t x, y;              // Posiciones de la matriz que corresponden al jugador
    uint16_t vx, vy;            // Velocidad del jugador
    Color color, trailColor;    // Color del jugador y del rastro que deja
    uint64_t lastTurbo;         // Ticks en el turbo anterior
} Player;

static void loop();
static int checkCollision(Player * p);
static int update(Player * p);
static void initializeField();
static void setDirections(Player * p, uint8_t up, uint8_t left, uint8_t down, uint8_t right, uint8_t turbo);
static void draw(Player * p, int * collided);

Player _p1, _p2;
uint8_t _playing;
//uint16_t _width, _height;
uint8_t _field[FIELD_WIDTH_POSITIONS][FIELD_HEIGHT]; // Optimizacion de espacio en bits

void startTron() {
    //uint32_t res = getScreenResolution();
    //_width = res & 0x0000FFFF;
    //_height = res >> 16; 

    initializeField();
    clear();
    puts("Bienvenido a tron\n");
    puts("Instrucciones: bla bla bla\n");
    puts("Presione espacio para comenzar\n");
    while (getScanCode() != SPACEBAR)
        ;
    _playing = 1;
    uint64_t initialTicks = getTicks();
    _p1 = (Player){ FIELD_WIDTH_POSITIONS * 8 * 0.25, FIELD_HEIGHT/2, 1,  0, (Color){255,153,184}, (Color){255,0,184}, 0};
    _p2 = (Player){ FIELD_WIDTH_POSITIONS * 8 * 0.75, FIELD_HEIGHT/2, -1, 0, (Color){100,255,100}, (Color){100,150,100}, 0};
    uint64_t ticks = getTicks();
    uint64_t nextTicks;
    while (_playing) {
        nextTicks = getTicks();
        setDirections(&_p1, KEY_W, KEY_A, KEY_S, KEY_D, KEY_Z);
        setDirections(&_p2, KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT, KEY_SLASH);
        if (nextTicks - ticks > DELTA_TICK) {
            ticks = nextTicks;
            loop(_field);
        }
    }
    //clear();
    puts("Fin del Juego hm?!!!!\n");
}

static void initializeField() {
    for (int i = 1; i < FIELD_WIDTH_POSITIONS - 2; i++)
        for (int j = 1; j < FIELD_HEIGHT - 2; j++)
            _field[i][j] = 0x00;
    
    for(int i = 0; i < FIELD_WIDTH_POSITIONS-1; i++){
        _field[i][0] = 0xFF;
        _field[i][FIELD_HEIGHT-1] = 0xFF;
    }

    for(int j = 0; j < FIELD_HEIGHT-1; j++){
        _field[0][j] = 0x01;
        _field[FIELD_WIDTH_POSITIONS-1][j] = 0x20; // Para que quede mejor en pantalla
    }

}

static void setDirections(Player * p, uint8_t up, uint8_t left, uint8_t down, uint8_t right, uint8_t turbo) {
    uint8_t key = getScanCode();
    uint64_t ticks = getTicks();
    if (key == up && p->vy == 0) {
        p->vx = 0;     
        p->vy = -1;
    } else if (key == left && p->vx == 0) {
        p->vx = -1;
        p->vy = 0;
    } else if (key == down && p->vy == 0) {
        p->vx = 0;
        p->vy = 1;
    } else if (key == right && p->vx == 0) {
        p->vx = 1;
        p->vy = 0;
    } else if (key == turbo && getTicks() - p->lastTurbo - TURBO_TICKS >= TURBO_COOLDOWN) { // Si ya paso el cooldown y se toca se activa
        p->lastTurbo = ticks;
    }
}

static void loop() {
    drawRect(_p1.x*PLAYER_SIZE, _p1.y*PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, _p1.trailColor);
    drawRect(_p2.x*PLAYER_SIZE, _p2.y*PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, _p2.trailColor);
    int col1 = update(&_p1);
    int col2 = update(&_p2);
    if (col1 && col2)
        puts("Empate!\n");
    else if (col1)
        puts("Gano el jugador 2!\n");
    else if (col2)
        puts("Gano el jugador 1!\n");

}

static int checkCollision(Player * p){
    char pos = _field[p->x/8][p->y];
    if((pos & (1 << (p->x % 8)))) {
        _playing = 0;
        return 1;
    }
    return 0;
}

static int update(Player * p) {
    uint8_t ticks = getTicks();
    uint8_t realV = 1;
    int collided = 0;
    if (ticks - p->lastTurbo <= TURBO_TICKS)
        realV = SPEED;

    for (int i = 0; i < realV && !collided; i++) {
        p->x += p->vx;
        p->y += p->vy;
        draw(p, &collided);
    }
    return collided;
}

static void draw(Player * p, int * collided) {
    collided = checkCollision(p);
    _field[p->x / 8][p->y] |= (1 << (p->x % 8));
    drawRect(p->x * PLAYER_SIZE, p->y * PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE, p->color);
}