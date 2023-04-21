/*
 * display.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "display.h"

void LedOutput(void *p_arg)
{

	(void)&p_arg;
	RTOS_ERR err;

	while (DEF_TRUE)
	{
		flags = OSFlagPend(&AlertFlag,
						   (LED0 | LED1), 0,
						   (OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME + OS_OPT_PEND_BLOCKING), NULL, &err);

		if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE)
		{
			// Turn off both LEDs initially
			GPIO_PinOutClear(LED0_port, LED0_pin);
			GPIO_PinOutClear(LED1_port, LED1_pin);

			// Light up LED0 if the corresponding flag is set
			if (flags & LED0)
			{
				GPIO_PinOutSet(LED0_port, LED0_pin);
			}

			// Light up LED1 if the corresponding flag is set
			if (flags & LED1)
			{
				GPIO_PinOutSet(LED1_port, LED1_pin);
			}
		}
		else
		{
			// Handle error, if necessary
		}
	}
}

void LedOutput_Create()
{
	RTOS_ERR err;

	OSTaskCreate(&LedOutputTCB,			 /* Pointer to the task's TCB.  */
				 "Led Output Task.",	 /* Name to help debugging.     */
				 &LedOutput,			 /* Pointer to the task's code. */
				 DEF_NULL,				 /* Pointer to task's argument. */
				 LedOutput_PRIO,		 /* Task's priority.            */
				 &LedOutputStk[0],		 /* Pointer to base of stack.   */
				 (LedOutput_SIZE / 10u), /* Stack limit, from base.     */
				 LedOutput_SIZE,		 /* Stack size, in CPU_STK.     */
				 10u,					 /* Messages in task queue.     */
				 0u,					 /* Round-Robin time quanta.    */
				 DEF_NULL,				 /* External TCB data.          */
				 OS_OPT_TASK_STK_CHK,	 /* Task options.               */
				 &err);

	if (err.Code != RTOS_ERR_NONE)
	{
		/* Handle error on task creation. */
	}
}

//***********************************************************************************

// LCDOutput

//***********************************************************************************

void LCDOutput(void *p_arg)
{
	(void)&p_arg;
	RTOS_ERR err;

	// Initialize the LCD and graphics library
	GLIB_Context_t glibContext;

	// Initialize GLIB context
	GLIB_contextInit(&glibContext);

	while (DEF_TRUE)
	{
		// Clear the screen
		GLIB_clear(&glibContext);

		OSTimeDlyHMSM(0u, 0u, 0u, 150u, OS_OPT_TIME_HMSM_STRICT, &err); /* Delay for 150 ms */

		OSMutexPend(&PhysicsMutex, 0u, OS_OPT_PEND_BLOCKING, 0u, &err);

		// Draw the game elements on the screen
		// e.g., draw the platform, satchel charges, railgun shots, castle, and foundation
		// Replace the following example with actual drawing functions for your game elements
		GLIB_drawLine(&glibContext, x1, y1, x2, y2); // Example of drawing a line

		OSMutexPost(&PhysicsMutex, OS_OPT_POST_NONE, &err);

		// Refresh the screen to display the updated elements
		DMD_updateDisplay();
	}

	void LCDOutput_Create()
	{
		RTOS_ERR err;

		OSTaskCreate(&LCDOutputTCB,			 /* Pointer to the task's TCB.  */
					 "LCD Output Task.",	 /* Name to help debugging.     */
					 LCDOutput,				 /* Pointer to the task's code. */
					 DEF_NULL,				 /* Pointer to task's argument. */
					 LCDOutput_PRIO,		 /* Task's priority.            */
					 &LCDOutputStk[0],		 /* Pointer to base of stack.   */
					 (LCDOutput_SIZE / 10u), /* Stack limit, from base.     */
					 LCDOutput_SIZE,		 /* Stack size, in CPU_STK.     */
					 10u,					 /* Messages in task queue.     */
					 0u,					 /* Round-Robin time quanta.    */
					 DEF_NULL,				 /* External TCB data.          */
					 OS_OPT_TASK_STK_CHK,	 /* Task options.               */
					 &err);

		if (err.Code != RTOS_ERR_NONE)
		{
			/* Handle error on task creation. */
		}
	}
	//***********************************************************************************

	// LCD GLIB INIT

	//***********************************************************************************

	void drawThickLine(GLIB_Context_t * glibContext, int x1, int y1, int x2, int y2,
					   int thickness)
	{
		int dx = x2 - x1;
		int dy = y2 - y1;
		//	int d;
		int i;

		if (abs(dx) > abs(dy))
		{
			//		d = dx >= 0 ? 1 : -1;
			for (i = -thickness / 2; i < thickness / 2 + thickness % 2; i++)
			{
				GLIB_drawLine(glibContext, x1, y1 + i, x2, y2 + i);
			}
		}
		else
		{
			//		d = dy >= 0 ? 1 : -1;
			for (i = -thickness / 2; i < thickness / 2 + thickness % 2; i++)
			{
				GLIB_drawLine(glibContext, x1 + i, y1, x2 + i, y2);
			}
		}
	}

	void Straight(GLIB_Context_t * glibContext)
	{

		int thickness = 10;

		// Arrow body
		drawThickLine(glibContext, 60, 40, 60, 120, thickness);
	}
