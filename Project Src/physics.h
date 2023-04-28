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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include"constant.h"
#include"platform.h"
#include"satchel.h"

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


uint8_t railgun_shots = 5;
OS_SEM railgun_semaphore;
bool auto_cannon = true;
int railgun_fired = 0;
int shotX;
int shotY;

static OS_TCB railgunTCB;
static CPU_STK railgunSTK[STACK_SIZES];

extern uint8_t PLATFORM_COUNT;
// extern struct PlatformData Platforms[];
extern OS_MUTEX platform_mutex;
extern int score;

// TODO: create queue for all the physics var
//***********************************************************************************
// function prototypes
//***********************************************************************************
void Physics_Task(void *p_arg);
void Physics_Task_Create();


void applyForceToPlatform(int32_t force);
void updatePlatformPosition(float deltaTime);
void updateSatchelCharges(float deltaTime);
void updateRailGunShots(float deltaTime);
void checkCollisions();

#endif /* PHYSICS_H_ */
