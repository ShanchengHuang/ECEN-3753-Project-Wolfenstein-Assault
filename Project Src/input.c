/*
 * input.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "input.h"

// Write to the FIFO
void button_event_fifo_write(ButtonEventFifo *fifo, int button_id,
							 int button_state)
{
	if (TwoButtonsPressed(fifo))
	{
		return;
	}

	if (fifo->count < BUTTON_FIFO_SIZE)
	{
		fifo->buffer[fifo->tail].button_id = button_id;
		fifo->buffer[fifo->tail].button_state = button_state;
		fifo->tail = (fifo->tail + 1) % BUTTON_FIFO_SIZE;
		fifo->count++;
	}
}

// Read from the FIFO
ButtonEvent button_event_fifo_read(ButtonEventFifo *fifo)
{
	ButtonEvent event;

	if (fifo->count > 0)
	{
		event = fifo->buffer[fifo->head];
		fifo->head = (fifo->head + 1) % BUTTON_FIFO_SIZE;
		fifo->count--;
	}
	else
	{
		event.button_id = -1; // Indicate an empty FIFO
	}

	return event;
}

bool TwoButtonsPressed(ButtonEventFifo *fifo)
{
	if (fifo->count < 2)
	{
		return false;
	}

	int lastIndex = fifo->tail - 1;
	if (lastIndex < 0)
	{
		lastIndex += BUTTON_FIFO_SIZE;
	}

	int secondLastIndex = lastIndex - 1;
	if (secondLastIndex < 0)
	{
		secondLastIndex += BUTTON_FIFO_SIZE;
	}

	ButtonEvent lastEvent = fifo->buffer[lastIndex];
	ButtonEvent secondLastEvent = fifo->buffer[secondLastIndex];

	if (lastEvent.button_state == 1 || secondLastEvent.button_state == 1)
	{
		return true;
	}

	return false;
}

// Instantiate a FIFO data structure for each of the buttons
static ButtonEventFifo fifo;

static ButtonEvent btn;

// Instantiate one instance of Vehicle Speed Data
static SpeedSetpointData vehicle_speed;

// Instantiate one instance of Vehicle Direction Data
static VehicleDirectionData vehicle_direction;

#define TIME_THRESHOLD 10
#define SPEED_THRESHOLD 75

void Button_Task(void *p_arg)
{
	/* Use argument. */
	(void)&p_arg;
	RTOS_ERR err;

	while (DEF_TRUE)
	{
		// SemPended
		OSSemPend(&BTNsemaphore, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
		// TODO
		//  Unit test here, do a print

		OSMutexPend(&BTNMutex, 0u, OS_OPT_PEND_BLOCKING, 0u, &err);
		//  Get date from cap
		OSMutexPost(&BTNMutex, OS_OPT_POST_NONE, &err);

		OSMutexPend(&PhysicsMutex, 0u, OS_OPT_PEND_BLOCKING, 0u, &err);
		// Push date to the physics queue
		OSMutexPost(&PhysicsMutex, OS_OPT_POST_NONE, &err);
	}
}

void Button_Task_Create()
{
	RTOS_ERR err;

	OSTaskCreate(&Button_TaskTCB,				/* Pointer to the task's TCB.  */
				 "Button_Task.",				/* Name to help debugging.     */
				 &Speed_Setpoint_Task,			/* Pointer to the task's code. */
				 DEF_NULL,						/* Pointer to task's argument. */
				 CapSensense_Task_PRIO,			/* Task's priority.            */
				 &Button_TaskStk[0],			/* Pointer to base of stack.   */
				 (CapSensense_Task_SIZE / 10u), /* Stack limit, from base.     */
				 CapSensense_Task_SIZE,			/* Stack size, in CPU_STK.     */
				 10u,							/* Messages in task queue.     */
				 0u,							/* Round-Robin time quanta.    */
				 DEF_NULL,						/* External TCB data.          */
				 OS_OPT_TASK_STK_CHK,			/* Task options.               */
				 &err);

	if (err.Code != RTOS_ERR_NONE)
	{
		/* Handle error on task creation. */
	}
}

void CapSensense_Task(void *p_arg)
{
	/* Use argument. */
	(void)&p_arg;
	RTOS_ERR err;

	while (DEF_TRUE)
	{
		// OS timer
		OSTimeDlyHMSM(0u, 0u, 0u, 50u, OS_OPT_TIME_HMSM_STRICT, &err); /* Delay for 50 ms */
		// TODO
		//  Get date from cap
		OSMutexPend(&PhysicsMutex, 0u, OS_OPT_PEND_BLOCKING, 0u, &err);
		// update in to the physics queue
		
		OSMutexPost(&PhysicsMutex, OS_OPT_POST_NONE, &err); /* Release the shared resource */
															//  Physics Mutex post here
															//  Update the capsence stats to the physics queue

		if (err != OS_ERR_NONE)
		{
			printf("Error: CapSensense_Task()", err);
		}
	}
}
void CapSensense_Task_Create()
{

	RTOS_ERR err;

	OSTaskCreate(&CapSensense_TaskTCB,			/* Pointer to the task's TCB.  */
				 "CapSensense_Task.",			/* Name to help debugging.     */
				 &CapSensense_Task,				/* Pointer to the task's code. */
				 DEF_NULL,						/* Pointer to task's argument. */
				 CapSensense_Task_PRIO,			/* Task's priority.            */
				 &CapSensense_TaskStk[0],		/* Pointer to base of stack.   */
				 (CapSensense_Task_SIZE / 10u), /* Stack limit, from base.     */
				 CapSensense_Task_SIZE,			/* Stack size, in CPU_STK.     */
				 10u,							/* Messages in task queue.     */
				 0u,							/* Round-Robin time quanta.    */
				 DEF_NULL,						/* External TCB data.          */
				 OS_OPT_TASK_STK_CHK,			/* Task options.               */
				 &err);

	if (err.Code != RTOS_ERR_NONE)
	{
		/* Handle error on task creation. */
	}
}

volatile int button0_state = 0; // 0: released, 1: pressed
volatile int button1_state = 0; // 0: released, 1: pressed

void GPIO_EVEN_IRQHandler(void)
{
	// reset the flag
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	// TODO
	//  Could be different, Need to check
	int state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);

	if (state == 0 && button1_state == 0)
	{ // Button0 is pressed and Button1 is released
		button0_state = 1;
		// Perform the action associated with Button0 press
	}
	else if (state == 1)
	{ // Button0 is released
		button0_state = 0;
		// Perform the action associated with Button0 release
	}

	if (button1_state)
	{
		return;
	}

	// Update speed setpoint
	btn.button_id = 0;

	if (btn.button_state)
	{
		btn.button_state = 0;
	}
	else
	{
		btn.button_state = 1;
	}

	// Write the button event to the FIFO
	button_event_fifo_write(&fifo, btn.button_id, btn.button_state);

	// Signal the semaphore
	RTOS_ERR err;
	OSSemPost(&BTNsemaphore, OS_OPT_POST_1, &err);
}

void GPIO_ODD_IRQHandler(void)
{
	// reset the flag
	uint32_t int_flag;
	int_flag = (GPIO->IF) & (GPIO->IEN);
	GPIO->IFC = int_flag;

	// TODO
	//  Could be different, Need to check
	//  Get the button state
	int state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);

	if (state == 0 && button0_state == 0)
	{ // Button1 is pressed and Button0 is released
		button1_state = 1;
		// Perform the action associated with Button1 press
	}
	else if (state == 1)
	{ // Button1 is released
		button1_state = 0;
		// Perform the action associated with Button1 release
	}

	if (button0_state)
	{
		return;
	}

	// Update speed setpoint
	btn.button_id = 1;

	if (btn.button_state)
	{
		btn.button_state = 0;
	}
	else
	{
		btn.button_state = 1;
	}

	// Write the button event to the FIFO
	button_event_fifo_write(&fifo, btn.button_id, btn.button_state);

	// Signal the semaphore
	RTOS_ERR err;
	OSSemPost(&BTNsemaphore, OS_OPT_POST_1, &err);
}
