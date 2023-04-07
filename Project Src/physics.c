/*
 * physics.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */


#include "physics.h"


void Physics_Task(void *p_arg) {

}
void Physics_Task_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Speed_SetpointTCB, /* Pointer to the task's TCB.  */
	"Speed_Setpoint_Task.", /* Name to help debugging.     */
	&Speed_Setpoint_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	Speed_Setpoint_PRIO, /* Task's priority.            */
	&Speed_SetpointStk[0], /* Pointer to base of stack.   */
	(Speed_Setpoint_SIZE / 10u), /* Stack limit, from base.     */
	Speed_Setpoint_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}
