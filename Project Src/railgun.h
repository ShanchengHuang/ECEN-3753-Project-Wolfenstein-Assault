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
//#include "platform.h"


#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//***********************************************************************************
// global variables
//***********************************************************************************

#define SHIELD_ACTIVE_100MS 5
#define SHIELD_RECHARGE_100MS 15

#define PASSIVE_KINETIC_REDUCTION -0.75
#define ACTIVE_KINETIC_GAIN -1.25

struct ShieldState {
  bool active;
  bool recharging;
};

struct ShieldState shield_state;

OS_Q shield_msg;
OS_TMR shieldRechargeTimer;

static OS_TMR shieldActiveTimer;
static OS_TCB shieldTCB;
static CPU_STK shieldSTK[STACK_SIZES];
static OS_MUTEX shield_mutex;


// Task stack size and priority
// #define Physics_Task_STK_SIZE 256u
// #define Physics_Task_PRIO 21u

// // Task stack
// static CPU_STK Physics_TaskStk[Physics_Task_STK_SIZE];

// // Timer
// static OS_TCB Physics_TaskTCB;


//***********************************************************************************
// function prototypes
//***********************************************************************************

void railgun_task_create(void);
void railgun_task(void);
bool shoot_railgun(int idx);

void shield_task(void);
void shield_task_create(void);



#endif /* RAILGUN_H_ */


