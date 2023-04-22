

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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//***********************************************************************************
// global variables
//***********************************************************************************
// Task stack size and priority
#define Physics_Task_STK_SIZE 256u
#define Physics_Task_PRIO 21u

// Task stack
static CPU_STK Physics_TaskStk[Physics_Task_STK_SIZE];

// Timer
static OS_TCB Physics_TaskTCB;


//***********************************************************************************
// function prototypes
//***********************************************************************************

void railgun_task_create(void);
void railgun_task(void);
bool shoot_railgun(int idx);


#endif /* RAILGUN_H_ */
