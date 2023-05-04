/*******************************************************************************
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_H
#define APP_H

/******************************************************************************
 * Initialize application.
 ******************************************************************************/
void app_init(void);

// For OS
#include <os.h>
#include "em_emu.h"
#include "gpio.h"
#include "capsense.h"
#include "sl_board_control.h"
#include "em_assert.h"
#include "dmd.h"
// #include "efm32gg11b.h" //For EFM_ASSERT

// For memcpy()
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// From other header
#include "physics.h"
// #include "platform.h"
// #include "railgun.h"
// #include "satchel.h"
// #include "timer.h"
#include "display.h"
#include "glib.h"

//***********************************************************************************

// global variables

//***********************************************************************************



// static OS_TCB gameTCB;
// static CPU_STK gameSTK[STACK_SIZES];

static OS_TCB IdleTaskTCB;
static CPU_STK IdleTaskStk[STACK_SIZES];


#define Dly_tick 2

//***********************************************************************************
// function prototypes
//***********************************************************************************

// OS_TMR CapSenseTimer;

// //***********************************************************************************
// Function Prototype
// //***********************************************************************************

void app_init(void);
void LCD_init();

// Added Idle task();
void IdleTask_Create();
void IdleTask(void *p_arg);

void game_over();
void decrement_life();
void start_game();

void Game_management_task_create();
void update_difficulty(void);
void game_management_task(void);


#endif // APP_H
