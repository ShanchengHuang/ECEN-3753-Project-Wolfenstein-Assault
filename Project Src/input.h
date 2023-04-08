/*
 * input.h
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#ifndef INPUT_H_

#define INPUT_H_

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
static volatile bool slider0 = 0;
static volatile bool slider1 = 0;
static volatile bool btn0 = 0;
static volatile bool btn1 = 0;

// Task variables:

#define Button_Task_PRIO 21u
#define Button_Task_SIZE 256u /*   Stack size in CPU_STK.         */

#define CapSensense_Task_PRIO 21u
#define CapSensense_Task_SIZE 256u /*   Stack size in CPU_STK.         */

// MicriumOS Task Globals
OS_TCB CapSensense_TaskTCB; /*   Task Control Block.   */
CPU_STK CapSensense_TaskStk[CapSensense_Task_SIZE]; /*  Task Stack.*/

OS_TCB Button_TaskTCB; /*   Task Control Block.   */
CPU_STK Button_TaskStk[Button_Task_SIZE]; /*  Task Stack.*/

static ButtonEvent btn;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void Button_Task(void *p_arg);
void Button_Task_Create();

void CapSensense_Task(void *p_arg);
void CapSensense_Task_Create();

#endif /* INPUT_H_ */
