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
