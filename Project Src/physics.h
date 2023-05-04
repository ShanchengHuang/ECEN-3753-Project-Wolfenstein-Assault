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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "constant.h"
#include "platform.h"
#include "satchel.h"
#include "railgun.h"

//***********************************************************************************
// global variables
//***********************************************************************************
// Task stack size and priority
#define Physics_Task_STK_SIZE 256u
#define Physics_Task_PRIO 21u

#define GRAVITY GRAVITY_PIXELS * SCREEN_MM / SCREEN_PIXELS //How fast shoud it dorp
#define GAME_OVER_SPEED 25
#define PHYSICS_DELTA (1 / 10.0) // TODO change this match to  constant.h
#define GRAVITY_PIXELS 20 // G

// Task stack
static CPU_STK Physics_TaskStk[Physics_Task_STK_SIZE];
static OS_TCB Physics_TaskTCB;

//extern uint8_t railgun_shots;
//extern OS_SEM railgun_semaphore;
//extern int railgun_fired;

static OS_TCB railgunTCB;
static CPU_STK railgunSTK[STACK_SIZES];

extern OS_MUTEX platform_mutex;
//extern int score;


extern bool castle_hit;

// TODO: create queue for all the physics var
//***********************************************************************************
// function prototypes
//***********************************************************************************

void Physics_Task(void *p_arg);
void Physics_Task_Create();

void update_platform(struct PlatData *shared_data);
void updateSatchelCharges(struct SatchelData *Satchels);
void checkCollisions(struct SatchelData Satchels, struct PlatData platform_data,
                     struct ShieldState shieldDat);

void checkBulletHit(struct BulletData Bullet);
void updateRailGunShots(struct BulletData *Bullet);

#endif /* PHYSICS_H_ */
