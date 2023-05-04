#ifndef _PLARTFORM_H_
#define _PLARTFORM_H_

#include <math.h>
#include <stdio.h>

#include "capsense.h"
#include "stdbool.h"
#include "constant.h"
#include "os.h"

// TODO need to change to match with constant.h

#define PLATFORM_Y 128 - 5
#define PLATFORM_HEIGHT 5
#define PLATFORM_WIDTH 20

#define PLATFORM_PERIOD 1

extern int MAX_SPEED; // Update this line

struct PlatData
{
	double x;
	double vx;
	double ax;
};

extern struct PlatData platform_data;
extern OS_MUTEX platform_mutex;

// static OS_SEM platform_semaphore;

static OS_TCB platformTCB;
static CPU_STK platformSTK[STACK_SIZES];

extern int railgun_fired;
extern int shotX;
extern int shotY;

// extern int railgun_shots;

void platform_task(void);
void platform_task_create(void);
void platform_timer_cb(void);

#endif /* _PLARTFORM_H_ */
