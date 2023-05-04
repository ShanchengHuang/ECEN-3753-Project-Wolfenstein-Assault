/*******************************************************************************
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

/*******************************************************************************
 * Initialize application.
 ******************************************************************************/

#include "app.h"

int cursor_pos = 0;
char death_cause[32] = "";
int score = 0;
int high_score = 0;
int lives = 3;

// Default Configuration Values

void app_init(void)
{
	// Initialize GPIO
	gpio_open();

	// Initialize our capactive touch sensor driver!
	CAPSENSE_Init();

	// Initialize game element:
	railgun_task_create();
	shield_task_create();
	Physics_Task_Create();
	platform_task_create();

	// Display task:
	LCD_init();
	//	LedOutput_Create();

	// Idle task:
	// IdleTask_Create();

	start_game();

	// Game_management: // implement when have more time
	// Game_management_task_create();
}

void game_over()
{

	//	gameState = GAME_OVER;

	// Turn off led
	GPIO_PinOutClear(LED1_port, LED1_pin);
	GPIO_PinOutClear(LED0_port, LED0_pin);

	while (1)
	{
		// inf loop for stops
	}
}

// void decrement_life()
// {
// 	lives--;
// 	if (lives == 0)
// 	{
// 		game_over("Satchel has exploded");
// 	}
// }

void start_game()
{
	platform_data.ax = 0;
	platform_data.vx = 0;
	platform_data.x = SCREEN_PIXELS / 2;

	satchel_init();
}

// If have more time
// void Game_management_task_create(void) {
 
// }


// //***********************************************************************************

// IdleTask

// //***********************************************************************************

void IdleTask(void *p_arg)
{
	/* Use argument. */
	(void)&p_arg;
	RTOS_ERR err;

	while (DEF_TRUE)
	{
		EMU_EnterEM1();

//		if (err.Code != RTOS_ERR_NONE)
//		{
//
//		}
	}
}

void IdleTask_Create()
{
	RTOS_ERR err;

	OSTaskCreate(&IdleTaskTCB,		  /* Pointer to the task's TCB.  */
				 "IdleTask",		  /* Name to help debugging.     */
				 &IdleTask,			  /* Pointer to the task's code. */
				 DEF_NULL,			  /* Pointer to task's argument. */
				 NORMAL_PRIORITY,	  /* Task's priority.            */
				 &IdleTaskStk[0],	  /* Pointer to base of stack.   */
				 (STACK_SIZES / 10u), /* Stack limit, from base.     */
				 STACK_SIZES,		  /* Stack size, in CPU_STK.     */
				 10u,				  /* Messages in task queue.     */
				 0u,				  /* Round-Robin time quanta.    */
				 DEF_NULL,			  /* External TCB data.          */
				 OS_OPT_TASK_STK_CHK, /* Task options.               */
				 &err);

	if (err.Code != RTOS_ERR_NONE)
	{
		/* Handle error on task creation. */
	}
}

// //***********************************************************************************

// //***********************************************************************************
