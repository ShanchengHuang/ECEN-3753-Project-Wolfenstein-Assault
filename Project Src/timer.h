/*
 * timer.h
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#ifndef TIMER_H_
#define TIMER_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "sl_board_control.h"
#include "em_assert.h"
#include "em_emu.h"
#include "dmd.h"
#include "os.h"

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
#define APP_TASK_START_STK_SIZE 128
#define APP_TASK_START_PRIO 1

// Task stack
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

// Timer
static OS_TMR LEDTimer;

//***********************************************************************************
// function prototypes
//***********************************************************************************
static void AppTaskStart(void *p_arg);
static void TimerCallback(void *p_arg);

void Timer_Task_Create();

#endif /* TIMER_H_ */
