/*
 * input.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "input.h"

void Button_Task(void *p_arg) {
	/* Use argument. */
	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		// TODO
	}

}

void Button_Task_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Button_TaskTCB, /* Pointer to the task's TCB.  */
	"Button_Task.", /* Name to help debugging.     */
	&Speed_Setpoint_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	CapSensense_Task_PRIO, /* Task's priority.            */
	&Button_TaskStk[0], /* Pointer to base of stack.   */
	(CapSensense_Task_SIZE / 10u), /* Stack limit, from base.     */
	CapSensense_Task_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}

void CapSensense_Task(void *p_arg) {
	/* Use argument. */
	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		// TODO
	}

}
void CapSensense_Task_Create() {

	RTOS_ERR err;

	OSTaskCreate(&CapSensense_TaskTCB, /* Pointer to the task's TCB.  */
	"CapSensense_Task.", /* Name to help debugging.     */
	&CapSensense_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	CapSensense_Task_PRIO, /* Task's priority.            */
	&CapSensense_TaskStk[0], /* Pointer to base of stack.   */
	(CapSensense_Task_SIZE / 10u), /* Stack limit, from base.     */
	CapSensense_Task_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}

}

volatile int button0_state = 0; // 0: released, 1: pressed
volatile int button1_state = 0; // 0: released, 1: pressed

void GPIO_EVEN_IRQHandler(void) {
	// reset the flag
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	// TODO

}

void GPIO_ODD_IRQHandler(void) {
	// reset the flag
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	// TODO
}

