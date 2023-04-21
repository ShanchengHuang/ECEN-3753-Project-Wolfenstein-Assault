/*
 * physics.h
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

//***********************************************************************************
// Include files
//***********************************************************************************
// For OS level lib
#include <os.h>
#include "em_emu.h"
#include "gpio.h"
#include "capsense.h"

// For memcpy()
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

// TODO: create queue for all the physics var
//***********************************************************************************
// function prototypes
//***********************************************************************************
void Physics_Task(void *p_arg);
void Physics_Task_Create();

#endif /* PHYSICS_H_ */
