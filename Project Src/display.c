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
	OS_FLAGS flags;

	while (DEF_TRUE) {

		flags = OSFlagPend(&AlertFlag,
				(SPEEDEDFLAG | RSET_SPEEDEDFLAG | DIRECTIONFLAG
						| RSET_DIRECTIONFLAG), 0,
				(OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
						+ OS_OPT_PEND_BLOCKING ), NULL, &err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

		//Had to reverse due to the flag
		if (flags & SPEEDEDFLAG) {
			// Handle case for FLAG1
			GPIO_PinOutSet(LED0_port, LED0_pin);
		} else if (flags & RSET_SPEEDEDFLAG) {
			GPIO_PinOutClear(LED0_port, LED0_pin);
		}

		if (flags & DIRECTIONFLAG) {
			// Handle case for FLAG2
			GPIO_PinOutSet(LED1_port, LED1_pin);
		} else if (flags & RSET_DIRECTIONFLAG) {
			GPIO_PinOutClear(LED1_port, LED1_pin);
		}

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

	VehicleDirectionData current_direction;
	SpeedSetpointData current_speed;

	const char *direction_str;

	char speed_str[20];

	while (DEF_TRUE) {
		//delate
		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);

		// Get the direction
		OSMutexPend(&DirectMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
		current_direction = vehicle_direction;
//		(void) &current_direction;
		switch (vehicle_direction.current_direction) {
		case 0:
			direction_str = "Hard Left";
			break;
		case 1:
			direction_str = "Slight Left";
			break;
		case 2:
			direction_str = "Slight Right";
			break;
		case 3:
			direction_str = "Hard Right";
			break;
		default:
			direction_str = "Straight";
			break;
		}
		OSMutexPost(&DirectMutex, OS_OPT_POST_NONE, &err);

		//Get the speed
		OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
		current_speed = vehicle_speed;
		(void) &current_speed;
		snprintf(speed_str, sizeof(speed_str), "Speed: %d mph",
				vehicle_speed.speed);
		OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);

		/* Fill lcd with background color */
		GLIB_clear(&glibContext);

		/* Use Normal font */
		GLIB_setFont(&glibContext, (GLIB_Font_t*) &GLIB_FontNormal8x8);

		/* Draw text on the memory lcd display*/
		GLIB_drawStringOnLine(&glibContext, direction_str, 0, GLIB_ALIGN_LEFT,
				5, 5,
				true);

		switch (current_direction.current_direction) {
		case 0:
			HardLeftTurnArrow(&glibContext);
			break;
		case 1:
			SlightLeftTurnArrow(&glibContext);
			break;
		case 2:
			SlightRightTurnArrow(&glibContext);
			break;
		case 3:
			HardRightTurnArrow(&glibContext);
			break;
		default:
			Straight(&glibContext);
			break;
		}
		/* Draw text on the memory lcd display*/
		GLIB_drawStringOnLine(&glibContext, speed_str, 1, GLIB_ALIGN_LEFT, 5, 5,
		true);
		/* Post updates to display */
		DMD_updateDisplay();

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
