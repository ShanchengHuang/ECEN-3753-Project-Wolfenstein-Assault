#include<math.h>
#include<stdio.h>

#include "capsense.h"
#include "stdbool.h"
#include "constant.h"
#include "os.h"

// TODO need to change to match with constant.h
#define PLATFORM_Y SCREEN_PIXELS - 15
#define PLATFORM_HEIGHT 5
#define PLATFORM_WIDTH 20

#define PLATFORM_MASS 10                                     // kg
#define MAX_PIXEL_FORCE 100                                  // kg px/s^2
#define MAX_FORCE MAX_PIXEL_FORCE *SCREEN_MM / SCREEN_PIXELS // kg * px/s^s
#define MAX_PIXEL_ACCEL MAX_PIXEL_FORCE / PLATFORM_MASS

struct PlatData{
	double x;
	double vx;
	double ax;
};


OS_MUTEX platform_mutex;

//static OS_SEM platform_semaphore;

static OS_TCB platformTCB;
static CPU_STK platformSTK[STACK_SIZES];


extern int railgun_fired;
extern int shotX;
extern int shotY;

//extern int railgun_shots;

void platform_task(void);
void platform_task_create(void);
void platform_timer_cb(void);
