/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/

// For display
#include "glib.h"


#include "sl_board_control.h"
#include "em_assert.h"
#include "dmd.h"
#include "os.h"

#include "app.h"

#include "input.h"
#include "timer.h"
#include "display.h"
#include "physics.h"
//
//#define Speed_Setpoint_PRIO 21u
//#define Speed_Setpoint_SIZE 256u /*   Stack size in CPU_STK.         */
//
//#define Vehicle_Direction_PRIO 21u
//#define Vehicle_Direction_SIZE 256u /*   Stack size in CPU_STK.         */
//
//#define Vehicle_Monitor_PRIO 21u
//#define Vehicle_Monitor_SIZE 256u
//
//#define Vehicle_Direction_PRIO 21u
//#define Vehicle_Direction_SIZE 256u /*   Stack size in CPU_STK.         */
//
//#define LedOutput_PRIO 22u
//#define LedOutput_SIZE 256u /*   Stack size in CPU_STK.         */
//
//#define LCDOutput_PRIO 22u
//#define LCDOutput_SIZE 256u
//// For Idle task
//#define IdleTask_PRIO 23u  // the lowest prio for Idle task
//#define IdleTask_SIZE 256u /*   Stack size in CPU_STK.         */
//
//// MicriumOS Task Globals
//OS_TCB Speed_SetpointTCB; /*   Task Control Block.   */
//CPU_STK Speed_SetpointStk[Speed_Setpoint_SIZE]; /*   Stack.                */
//
//OS_TCB Vehicle_DirectionTCB; /*   Task Control Block.   */
//CPU_STK Vehicle_DirectionStk[Vehicle_Direction_SIZE]; /*   Stack.                */
//
//OS_TCB LedOutputTCB; /*   Task Control Block.   */
//CPU_STK LedOutputStk[LedOutput_SIZE]; /*   Stack.                */
//
//OS_TCB LCDOutputTCB; /*   Task Control Block.   */
//CPU_STK LCDOutputStk[LCDOutput_SIZE]; /*   Stack.                */
//
//OS_TCB Vehicle_MonitorTCB; /*   Task Control Block.   */
//CPU_STK Vehicle_MonitorStk[Vehicle_Monitor_SIZE]; /*   Stack.                */
//
//OS_TCB IdleTaskTCB; /*   Task Control Block.   */
//CPU_STK IdleTaskStk[IdleTask_SIZE]; /*   Stack.                */
//

// Define the timer possibly

void app_init(void) {
	// Initialize GPIO
	gpio_open();

	// Initialize our capactive touch sensor driver!
	CAPSENSE_Init();

	// Initialize our LCD system
	LCD_init();

	// Init all the flag mutex sem
	OSFlag_Init();

	Speed_Setpoint_Create();
	Vehicle_Direction_Create();
	Vehicle_Monitor_Create();
	LedOutput_Create();
	LCDOutput_Create();
	IdleTask_Create();

}



void LCD_init() {
	uint32_t status;
	/* Enable the memory lcd */
	status = sl_board_enable_display();
	EFM_ASSERT(status == SL_STATUS_OK);

	/* Initialize the DMD support for memory lcd display */
	status = DMD_init(0);
	EFM_ASSERT(status == DMD_OK);

	/* Initialize the glib context */
	status = GLIB_contextInit(&glibContext);
	EFM_ASSERT(status == GLIB_OK);

	glibContext.backgroundColor = White;
	glibContext.foregroundColor = Black;

	/* Fill lcd with background color */
	GLIB_clear(&glibContext);

	/* Use Normal font */
	GLIB_setFont(&glibContext, (GLIB_Font_t*) &GLIB_FontNormal8x8);

	/* Draw text on the memory lcd display*/
	GLIB_drawStringOnLine(&glibContext, "Welcome to...\n**Lab 7**!", 0,
			GLIB_ALIGN_LEFT, 5, 5,
			true);

	/* Draw text on the memory lcd display*/
	GLIB_drawStringOnLine(&glibContext, "Review the lab\ninstructions!", 2,
			GLIB_ALIGN_LEFT, 5, 5,
			true);

//	SlightLeftTurnArrow(&glibContext);
	/* Post updates to display */
	DMD_updateDisplay();
}

//***********************************************************************************

// Shared data structures and function

//***********************************************************************************

// Write to the FIFO
void button_event_fifo_write(ButtonEventFifo *fifo, int button_id,
		int button_state) {

	if (TwoButtonsPressed(fifo)) {
		return;
	}

	if (fifo->count < BUTTON_FIFO_SIZE) {
		fifo->buffer[fifo->tail].button_id = button_id;
		fifo->buffer[fifo->tail].button_state = button_state;
		fifo->tail = (fifo->tail + 1) % BUTTON_FIFO_SIZE;
		fifo->count++;
	}
}

// Read from the FIFO
ButtonEvent button_event_fifo_read(ButtonEventFifo *fifo) {
	ButtonEvent event;

	if (fifo->count > 0) {
		event = fifo->buffer[fifo->head];
		fifo->head = (fifo->head + 1) % BUTTON_FIFO_SIZE;
		fifo->count--;
	} else {
		event.button_id = -1; // Indicate an empty FIFO
	}

	return event;
}

bool TwoButtonsPressed(ButtonEventFifo *fifo) {
	if (fifo->count < 2) {
		return false;
	}

	int lastIndex = fifo->tail - 1;
	if (lastIndex < 0) {
		lastIndex += BUTTON_FIFO_SIZE;
	}

	int secondLastIndex = lastIndex - 1;
	if (secondLastIndex < 0) {
		secondLastIndex += BUTTON_FIFO_SIZE;
	}

	ButtonEvent lastEvent = fifo->buffer[lastIndex];
	ButtonEvent secondLastEvent = fifo->buffer[secondLastIndex];

	if (lastEvent.button_state == 1 || secondLastEvent.button_state == 1) {
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

//***********************************************************************************

// Flag, Timer, Semaphore Mutex create here

//***********************************************************************************
void OSFlag_Init(void) {

	RTOS_ERR err;

	btn.button_state = 0;

	vehicle_speed.decrements = 5;
	vehicle_speed.increments = 5;

	vehicle_direction.current_direction = -1;

	// Create the event flag group
	OSFlagCreate(&UpdateFlag, "Speed update Flag Group", 0, &err);
	OSFlagCreate(&AlertFlag, "Alert event Group", 0, &err);

	// Create the periodic timer for sampling the touch slider
	//	OSTmrCreate(&CapSenseTimer, "CapSense Timer", 0, 1, OS_OPT_TMR_PERIODIC,
	//			&TimerCallback, NULL, &err);

	// Create the semaphore
	OSSemCreate(&BTNsemaphore, "Button Semaphore", 0, &err);

	// Create the Mutex
	OSMutexCreate(&SpeedMutex, "Speed Setpoint Mutex", &err);
	OSMutexCreate(&DirectMutex, "Vehicle Direction Mutex", &err);

	// Error check
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}

//***********************************************************************************

// Speed Setpoint Task

//***********************************************************************************

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

void Speed_Setpoint_Task(void *p_arg) {

	(void) &p_arg;
	RTOS_ERR err;
	ButtonEvent event;

	while (DEF_TRUE) {

		// Wait for the semaphore
		OSSemPend(&BTNsemaphore, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

		// Read button events from the FIFO and check if two pressed at same time
		if (TwoButtonsPressed(&fifo)) {
			event.button_id = -1;
			event.button_state = -1;
		} else {
			event = button_event_fifo_read(&fifo);
		}

		// Mutex to wirte
		OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

		if (event.button_id == 0 && event.button_state) {
			vehicle_speed.speed -= vehicle_speed.decrements;
		} else if (event.button_id == 1 && event.button_state) {
			vehicle_speed.speed += vehicle_speed.increments;
		}
		OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);

		//release flage to the update flag
		OSFlagPost(&UpdateFlag, UPDATEFLAG1, OS_OPT_POST_FLAG_SET, &err);

		// Error Check
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
	}
}

void Speed_Setpoint_Create() {

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

//***********************************************************************************

// Vehicle Direction Task

//***********************************************************************************

void Vehicle_Direction_Task(void *p_arg) {

	(void) &p_arg;
	RTOS_ERR err;
	int current_position;
//	VehicleDirectionData prev_vehicle_direction;

	while (DEF_TRUE) {
		// Delate for awakened periodically
		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);

		// Read the current position
		current_position = read_capsense2();

		// Mutex to wirte into direction
		OSMutexPend(&DirectMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

		// if there are changes in direction
		if (current_position != vehicle_direction.current_direction) {

			vehicle_direction.current_direction = current_position;

			//Clear time if dir changes
			vehicle_direction.time_constant = 0;

			// Update the number of turns
			if (current_position == 0 || current_position == 1) {
				vehicle_direction.left_turns += 1;
			} else if (current_position == 2 || current_position == 3) {
				vehicle_direction.right_turns += 1;
			}

			OSFlagPost(&UpdateFlag, UPDATEFLAG2, OS_OPT_POST_FLAG_SET, &err);

		} else if (current_position != -1) {
			// no direction changes, and not in straight
			vehicle_direction.time_constant += 1;

		}

		OSMutexPost(&DirectMutex, OS_OPT_POST_NONE, &err);

	}
}

void Vehicle_Direction_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Vehicle_DirectionTCB, /* Pointer to the task's TCB.  */
	"Vehicle Direction Task.", /* Name to help debugging.     */
	&Vehicle_Direction_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	Vehicle_Direction_PRIO, /* Task's priority.            */
	&Vehicle_DirectionStk[0], /* Pointer to base of stack.   */
	(Vehicle_Direction_SIZE / 10u), /* Stack limit, from base.     */
	Vehicle_Direction_SIZE, /* Stack size, in CPU_STK.     */
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

// Vehicle Monitor Task

//***********************************************************************************

void Vehicle_Monitor_Task(void *p_arg) {

	/* Use argument. */
	(void) &p_arg;

	RTOS_ERR err;
	bool speed_violation;
	bool direction_violation;

	while (DEF_TRUE) {

		OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);

		// Flag pend for update
		OSFlagPend(&UpdateFlag, UPDATEFLAG1 | UPDATEFLAG2, 0,
		OS_OPT_PEND_FLAG_SET_ANY, NULL, &err);

		// Mutex to read
		OSMutexPend(&DirectMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
		OSMutexPend(&SpeedMutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

		if (vehicle_direction.time_constant >= TIME_THRESHOLD) {
			direction_violation = true;
		} else {
			direction_violation = false;
		}

		// Mutex to read

		if (vehicle_speed.speed >= SPEED_THRESHOLD
				|| (vehicle_direction.current_direction != -1
						&& vehicle_speed.speed >= 45)) {
			speed_violation = true;
		} else {
			speed_violation = false;
		}

		OSMutexPost(&DirectMutex, OS_OPT_POST_NONE, &err);
		OSMutexPost(&SpeedMutex, OS_OPT_POST_NONE, &err);

		//  violation Check
		if (speed_violation) {
			OSFlagPost(&AlertFlag, (OS_FLAGS) SPEEDEDFLAG, OS_OPT_POST_FLAG_SET,
					&err);
		} else {
			OSFlagPost(&AlertFlag, (OS_FLAGS) RSET_SPEEDEDFLAG,
			OS_OPT_POST_FLAG_SET, &err);
		}

		if (direction_violation) {
			OSFlagPost(&AlertFlag, (OS_FLAGS) DIRECTIONFLAG,
			OS_OPT_POST_FLAG_SET, &err);
		} else {
			OSFlagPost(&AlertFlag, (OS_FLAGS) RSET_DIRECTIONFLAG,
			OS_OPT_POST_FLAG_SET, &err);
		}

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
	}
}

void Vehicle_Monitor_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Vehicle_MonitorTCB, /* Pointer to the task's TCB.  */
	"Vehicle Monitor Task.", /* Name to help debugging.     */
	&Vehicle_Monitor_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	Vehicle_Monitor_PRIO, /* Task's priority.            */
	&Vehicle_MonitorStk[0], /* Pointer to base of stack.   */
	(Vehicle_Monitor_SIZE / 10u), /* Stack limit, from base.     */
	Vehicle_Monitor_SIZE, /* Stack size, in CPU_STK.     */
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

// LedOutput

//***********************************************************************************

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

//***********************************************************************************

// IdleTask

//***********************************************************************************

void IdleTask(void *p_arg) {
	/* Use argument. */
	(void) &p_arg;

//    RTOS_ERR err;

	while (DEF_TRUE) {
		EMU_EnterEM1();
		// Not need for Time dely
		// OSTimeDly(Dly_tick, OS_OPT_TIME_DLY, &err);
	}
}

void IdleTask_Create() {
	RTOS_ERR err;

	OSTaskCreate(&IdleTaskTCB, /* Pointer to the task's TCB.  */
	"IdleTask", /* Name to help debugging.     */
	&IdleTask, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	IdleTask_PRIO, /* Task's priority.            */
	&IdleTaskStk[0], /* Pointer to base of stack.   */
	(IdleTask_SIZE / 10u), /* Stack limit, from base.     */
	IdleTask_SIZE, /* Stack size, in CPU_STK.     */
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

// Function from lab 2

//***********************************************************************************

void read_button0(void) {
	if (!GPIO_PinInGet(BUTTON0_port, BUTTON0_pin)) {
		btn0 = true;
	} else {
		btn0 = false;
	}
}

void read_button1(void) {
	if (!GPIO_PinInGet(BUTTON1_port, BUTTON1_pin)) {
		btn1 = true;
	} else {
		btn1 = false;
	}
}

void read_capsense(void) {
	CAPSENSE_Sense();

	if (CAPSENSE_getPressed(0) || CAPSENSE_getPressed(1)) {
		slider1 = true;
	} else {
		slider1 = false;
	}

	if (CAPSENSE_getPressed(2) || CAPSENSE_getPressed(3)) {
		slider0 = true;
	} else {
		slider0 = false;
	}
}

int read_capsense2(void) {
	CAPSENSE_Sense();

	if (CAPSENSE_getPressed(0)) {
		return 0;
	}

	if (CAPSENSE_getPressed(1)) {
		return 1;
	}

	if (CAPSENSE_getPressed(2)) {
		return 2;
	}

	if (CAPSENSE_getPressed(3)) {
		return 3;
	}

	return -1;

}

void write_led(void) {
	if ((btn0 || slider0) && !(btn1 || slider1)) { // Led0 on
		GPIO_PinOutSet(LED0_port, LED0_pin);
		GPIO_PinOutClear(LED1_port, LED1_pin);
	} else if (!(btn0 || slider0) && (btn1 || slider1)) { // Led1 on
		GPIO_PinOutSet(LED1_port, LED1_pin);
		GPIO_PinOutClear(LED0_port, LED0_pin);
	} else { // also all off
		GPIO_PinOutClear(LED0_port, LED0_pin);
		GPIO_PinOutClear(LED1_port, LED1_pin);
	}
}
