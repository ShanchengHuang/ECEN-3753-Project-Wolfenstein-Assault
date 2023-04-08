/*
 * display.h
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

//***********************************************************************************
// Include files
//***********************************************************************************
// Display
#include "glib.h"

// OS
#include "os.h"

//***********************************************************************************
// global variables
//***********************************************************************************
#define LedOutput_PRIO 22u
#define LedOutput_SIZE 256u /*   Stack size in CPU_STK.         */

#define LCDOutput_PRIO 22u
#define LCDOutput_SIZE 256u

OS_TCB LedOutputTCB; /*   Task Control Block.   */
CPU_STK LedOutputStk[LedOutput_SIZE]; /*   Stack.                */

OS_TCB LCDOutputTCB; /*   Task Control Block.   */
CPU_STK LCDOutputStk[LCDOutput_SIZE]; /*   Stack.                */

static GLIB_Context_t glibContext;
//***********************************************************************************
// function prototypes
//***********************************************************************************
void Dispaly_Task(void *p_arg);
void Dispaly_Task_Create();

void drawThickLine(GLIB_Context_t *glibContext, int x1, int y1, int x2, int y2,
		int thickness);
Straight(GLIB_Context_t *glibContext);

#endif /* DISPLAY_H_ */
