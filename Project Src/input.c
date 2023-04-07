/*
 * input.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

void Button_Task(void *p_arg) {

}
void Button_Task_Create() {
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

void CapSensense_Task(void *p_arg) {

}
void CapSensense_Task_Create() {

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

volatile int button0_state = 0; // 0: released, 1: pressed
volatile int button1_state = 0; // 0: released, 1: pressed

void GPIO_EVEN_IRQHandler(void) {
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	int state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);

	if (state == 0 && button1_state == 0) { // Button0 is pressed and Button1 is released
		button0_state = 1;
		// Perform the action associated with Button0 press
	} else if (state == 1) { // Button0 is released
		button0_state = 0;
		// Perform the action associated with Button0 release
	}

	if (button1_state) {
		return;
	}

	// Update speed setpoint
	btn.button_id = 0;

	if (btn.button_state) {
		btn.button_state = 0;
	} else {
		btn.button_state = 1;
	}

	// Write the button event to the FIFO
	button_event_fifo_write(&fifo, btn.button_id, btn.button_state);

	// Signal the semaphore
	RTOS_ERR err;
	OSSemPost(&BTNsemaphore, OS_OPT_POST_1, &err);

}

void GPIO_ODD_IRQHandler(void) {
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	// Get the button state
	int state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);

	if (state == 0 && button0_state == 0) { // Button1 is pressed and Button0 is released
		button1_state = 1;
		// Perform the action associated with Button1 press
	} else if (state == 1) { // Button1 is released
		button1_state = 0;
		// Perform the action associated with Button1 release
	}

	if (button0_state) {
		return;
	}

	// Update speed setpoint
	btn.button_id = 1;

	if (btn.button_state) {
		btn.button_state = 0;
	} else {
		btn.button_state = 1;
	}

	// Write the button event to the FIFO
	button_event_fifo_write(&fifo, btn.button_id, btn.button_state);

	// Signal the semaphore
	RTOS_ERR err;
	OSSemPost(&BTNsemaphore, OS_OPT_POST_1, &err);
}

