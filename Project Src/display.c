/*
 * display.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "display.h"

void LedOutput(void *p_arg) {

	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		//TODO
	}
}

void LedOutput_Create() {
	RTOS_ERR err;

	OSTaskCreate(&LedOutputTCB, /* Pointer to the task's TCB.  */
	"Led Output Task.", /* Name to help debugging.     */
	&LedOutput, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	LedOutput_PRIO, /* Task's priority.            */
	&LedOutputStk[0], /* Pointer to base of stack.   */
	(LedOutput_SIZE / 10u), /* Stack limit, from base.     */
	LedOutput_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}

//***********************************************************************************

// LCDOutput

//***********************************************************************************

void LCDOutput(void *p_arg) {

	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		// TODO
	}

}

void LCDOutput_Create() {
	RTOS_ERR err;

	OSTaskCreate(&LCDOutputTCB, /* Pointer to the task's TCB.  */
	"LCD Output Task.", /* Name to help debugging.     */
	LCDOutput, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	LCDOutput_PRIO, /* Task's priority.            */
	&LCDOutputStk[0], /* Pointer to base of stack.   */
	(LCDOutput_SIZE / 10u), /* Stack limit, from base.     */
	LCDOutput_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}
//***********************************************************************************

// LCD GLIB INIT

//***********************************************************************************



void drawThickLine(GLIB_Context_t *glibContext, int x1, int y1, int x2, int y2,
		int thickness) {
	int dx = x2 - x1;
	int dy = y2 - y1;
//	int d;
	int i;

	if (abs(dx) > abs(dy)) {
//		d = dx >= 0 ? 1 : -1;
		for (i = -thickness / 2; i < thickness / 2 + thickness % 2; i++) {
			GLIB_drawLine(glibContext, x1, y1 + i, x2, y2 + i);
		}
	} else {
//		d = dy >= 0 ? 1 : -1;
		for (i = -thickness / 2; i < thickness / 2 + thickness % 2; i++) {
			GLIB_drawLine(glibContext, x1 + i, y1, x2 + i, y2);
		}
	}
}


void Straight(GLIB_Context_t *glibContext) {

	int thickness = 10;

	// Arrow body
	drawThickLine(glibContext, 60, 40, 60, 120, thickness);

}
