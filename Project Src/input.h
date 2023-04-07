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

#define Speed_Setpoint_PRIO 21u
#define Speed_Setpoint_SIZE 256u /*   Stack size in CPU_STK.         */

#define Vehicle_Direction_PRIO 21u
#define Vehicle_Direction_SIZE 256u /*   Stack size in CPU_STK.         */

#define Vehicle_Monitor_PRIO 21u
#define Vehicle_Monitor_SIZE 256u

#define Vehicle_Direction_PRIO 21u
#define Vehicle_Direction_SIZE 256u /*   Stack size in CPU_STK.         */

#define LedOutput_PRIO 22u
#define LedOutput_SIZE 256u /*   Stack size in CPU_STK.         */

#define LCDOutput_PRIO 22u
#define LCDOutput_SIZE 256u

// For Idle task
#define IdleTask_PRIO 23u  // the lowest prio for Idle task
#define IdleTask_SIZE 256u /*   Stack size in CPU_STK.         */

// MicriumOS Task Globals
OS_TCB Speed_SetpointTCB; /*   Task Control Block.   */
CPU_STK Speed_SetpointStk[Speed_Setpoint_SIZE]; /*   Stack.                */

OS_TCB Vehicle_DirectionTCB; /*   Task Control Block.   */
CPU_STK Vehicle_DirectionStk[Vehicle_Direction_SIZE]; /*   Stack.                */

OS_TCB LedOutputTCB; /*   Task Control Block.   */
CPU_STK LedOutputStk[LedOutput_SIZE]; /*   Stack.                */

OS_TCB LCDOutputTCB; /*   Task Control Block.   */
CPU_STK LCDOutputStk[LCDOutput_SIZE]; /*   Stack.                */

OS_TCB Vehicle_MonitorTCB; /*   Task Control Block.   */
CPU_STK Vehicle_MonitorStk[Vehicle_Monitor_SIZE]; /*   Stack.                */

OS_TCB IdleTaskTCB; /*   Task Control Block.   */
CPU_STK IdleTaskStk[IdleTask_SIZE]; /*   Stack.                */

static ButtonEvent btn;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void Button_Task(void *p_arg);
void Button_Task_Create();

void CapSensense_Task(void *p_arg);
void CapSensense_Task_Create();

#endif /* INPUT_H_ */
