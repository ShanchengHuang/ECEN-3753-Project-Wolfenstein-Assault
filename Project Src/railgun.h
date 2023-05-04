#ifndef RAILGUN_H_
#define RAILGUN_H_

//***********************************************************************************
// Include files
//***********************************************************************************
// For OS level lib
#include <os.h>
#include "em_emu.h"
#include "gpio.h"
#include "capsense.h"
#include "constant.h"
#include "platform.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//***********************************************************************************
// global variables
//***********************************************************************************

struct ShieldState {
	bool active;
	int recharging;
};

extern struct ShieldState shield_state;

extern int railgun_fired;

static OS_TCB shieldTCB;
static CPU_STK shieldSTK[STACK_SIZES];

extern  OS_MUTEX shield_mutex;
extern  OS_MUTEX railgun_mutex;


struct BulletData {
  double vx;
  double vy;
  double x;
  double y;
};

extern struct BulletData Bullet;

//***********************************************************************************
// function prototypes
//***********************************************************************************

void railgun_task_create(void);
void railgun_task(void);
bool shoot_railgun(int idx);

void shield_task(void);
void shield_task_create(void);

bool read_button0(void);
bool read_button1(void);

#endif /* RAILGUN_H_ */

