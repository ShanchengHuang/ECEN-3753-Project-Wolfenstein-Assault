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

void app_init(void) {
	// Initialize GPIO
	gpio_open();

	// Initialize our capactive touch sensor driver!
	CAPSENSE_Init();

	// Initialize game element:
	railgun_task_create(); // it should has both Shild and Railgun task
	Physics_Task_Create();
	platform_task_create();

	// Display task:
	LCDOutput_Create();
	LedOutput_Create();

	// Idle task:
	IdleTask_Create();

	// Game_management:
	Game_management_task_create();
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

	// Game start up
	GLIB_drawStringOnLine(&glibContext, "   Wolfenstein\n     Assault", 1,
			GLIB_ALIGN_LEFT, 0, 10,
			true);

	/* Draw text on the memory lcd display*/
	GLIB_drawStringOnLine(&glibContext, " Press any key \n  To start", 1,
			GLIB_ALIGN_LEFT, 0, 50,
			true);

	//	SlightLeftTurnArrow(&glibContext);
	/* Post updates to display */
	DMD_updateDisplay();
}

void game_over(char cause[]) {

	gameState = GAME_OVER;

//	cursor_pos = 0;

	if (score > high_score)
		high_score = score;
	// TODO Print the Endgame

	//	turn_off_led();

	// Stop all the timer and flag

}

void decrement_life(void) {
	lives--;
	if (lives == 0) {
		game_over("Satchel has exploded");
	}
}

void start_game() {
	RTOS_ERR tmrErr;
	RTOS_ERR flgErr;

	platform_data.ax = 0;
	platform_data.vx = 0;
	platform_data.x = SCREEN_PIXELS / 2;

	// init the values
	// Default Configuration Values
	GameConfig default_config = {
	    .data_structure_version = 1,
	    .tau_physics = 50,
	    .tau_display = 150,
	    .tau_slider = 100,
	    .canyon_size = 100000,
	    .wolfenstein = {
	        .castle_height = 5000,
	        .foundation_hits_required = 2,
	        .foundation_depth = 5000,
	    },
	    .satchel_charges = {
	        .limiting_method = 0, .display_diameter = 10,
	        .tau_throw = 0, // Union with MaxNumInFlight
	    },
	    .platform = {
	        .max_force = 20000000,
	        .mass = 100,
	        .length = 10000,
	        .max_platform_bounce_speed = 50000,
	    },
	    .shield = {
	        .effective_range = 15000,
	        .activation_energy = 30000,
	    },
	    .railgun = {
	        .elevation_angle = 800,
	        .shot_mass = 50,
	        .shot_display_diameter = 5,
	    },
	    .generator = {
	        .energy_storage = 50000,
	        .power = 20000,
	    },
	};

	satchel_init();
	gameState = IN_PROGRESS;

	// Timer

//	OSTmrStart(&ledTmrs[0], &tmrErr);
//	if (tmrErr.Code != RTOS_ERR_NONE)
//		EFM_ASSERT(false);


	OSFlagPost(&game_state, IN_PROGRESS, OS_OPT_POST_FLAG_SET, &flgErr);
	if (tmrErr.Code != RTOS_ERR_NONE || flgErr.Code)
		EFM_ASSERT(false);

	OSTmrStart(&platform_timer, &tmrErr);

	OSFlagPost(&game_state, PREGAME | GAME_OVER, OS_OPT_POST_FLAG_CLR, &flgErr);
	if (flgErr.Code || tmrErr.Code)
		EFM_ASSERT(false);
}

void Game_management_task_create(void) {
//	RTOS_ERR tskErr;
//	RTOS_ERR flgErr;
//	RTOS_ERR qErr;
//
//	OSTaskCreate(&gameTCB, /* Pointer to the task's TCB.  */
//	"game Task.", /* Name to help debugging.     */
//	&game_management_task, /* Pointer to the task's code. */
//	DEF_NULL, /* Pointer to task's argument. */
//	ABOVE_NORMAL_PRIORITY, /* Task's priority.            */
//	&gameSTK[0], /* Pointer to base of stack.   */
//	(STACK_SIZES / 10u), /* Stack limit, from base.     */
//	STACK_SIZES, /* Stack size, in CPU_STK.     */
//	10u, /* Messages in task queue.     */
//	120u, /* Round-Robin time quanta.    */
//	DEF_NULL, /* External TCB data.          */
//	OS_OPT_TASK_STK_CHK, /* Task options.               */
//	&tskErr);
//
//	OSFlagCreate(&game_state, "game state flags", PREGAME, &flgErr);
//
//	OSQCreate(&btn_q, "Q for managing game state", 4, &qErr);
//
//	if (flgErr.Code || tskErr.Code || qErr.Code)
//		EFM_ASSERT(false);
}

void update_difficulty(void) {
	// TODO, adding it when I have time
}

void game_management_task(void) {
	RTOS_ERR flgErr;
	RTOS_ERR qErr;

	uint8_t *btnMsg;
	uint8_t *size;
	while (1) {
		while (OSFlagPend(&game_state, PREGAME | GAME_OVER, 0,
		OS_OPT_PEND_FLAG_SET_ANY, NULL, &flgErr) != IN_PROGRESS) { // When getting btn feaback star game

			// TODO adding semaphore
			OSQPend(&btn_q, 0, OS_OPT_PEND_BLOCKING, &size, NULL, &qErr);

			start_game();
		}
	}
}

// //***********************************************************************************

// IdleTask

// //***********************************************************************************

void IdleTask(void *p_arg) {
	/* Use argument. */
	(void) &p_arg;
	RTOS_ERR err;

	while (DEF_TRUE) {
		EMU_EnterEM1();

		if (err.Code != RTOS_ERR_NONE) {
		}
	}
}

void IdleTask_Create() {
	RTOS_ERR err;

	OSTaskCreate(&IdleTaskTCB, /* Pointer to the task's TCB.  */
	"IdleTask", /* Name to help debugging.     */
	&IdleTask, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	NORMAL_PRIORITY, /* Task's priority.            */
	&IdleTaskStk[0], /* Pointer to base of stack.   */
	(STACK_SIZES / 10u), /* Stack limit, from base.     */
	STACK_SIZES, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
		/* Handle error on task creation. */
	}
}

// //***********************************************************************************

// //***********************************************************************************
