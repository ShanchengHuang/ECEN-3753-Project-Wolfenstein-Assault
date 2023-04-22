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

#include "lcd.h"
#include "glib.h"
#include "dmd.h"
#include "sl_board_control.h"
#include "satchel_system/ss.h"
#include "platform_control/platform.h"
#include "shield_system/shield.h"
#include "game_management/game_management.h"
#include "math.h"
#include "string.h"

#define LCD_PERIOD 1

	OS_TMR lcdTmr;

	static OS_SEM lcd_semaphore;
	static OS_TCB lcdTCB;
	static CPU_STK lcdSTK[STACK_SIZES];
	static GLIB_Context_t glibContext;

	extern uint8_t SS_COUNT;
	extern struct SatchelData satchels[];
	extern OS_MUTEX ss_mutex;
	extern OS_MUTEX platform_mutex;
	extern struct PlatformData platform_data;
	extern struct ShieldState shield_state;
	extern uint8_t railgun_charges;
	extern enum difficulty_e difficulty;
	extern enum game_state_e gameState;
	extern int cursor_pos;
	extern int PLATFORM_BOUNCE_ENABLED;
	extern int score;
	extern int high_score;
	extern int lives;
	extern bool auto_railgun;
	extern int railgun_fired;
	extern char death_cause[];
	extern int shotX;
	extern int shotY;

	void lcd_timer_cb(void)
	{
		RTOS_ERR semPostErr;
		OSSemPost(
			&lcd_semaphore,
			OS_OPT_POST_1,
			&semPostErr);
		if (semPostErr.Code)
			EFM_ASSERT(false);
	}

	void LCD_init()
	{
		RTOS_ERR semErr;
		RTOS_ERR tmrErr;
		RTOS_ERR tskErr;
		OSSemCreate(
			&lcd_semaphore,
			"lcd Semaphore",
			0,
			&semErr);
		OSTmrCreate(
			&lcdTmr,
			"lcd timer",
			0,
			LCD_PERIOD,
			OS_OPT_TMR_PERIODIC,
			&lcd_timer_cb,
			NULL,
			&tmrErr);
		OSTaskCreate(
			&lcdTCB,			 /* Pointer to the task's TCB.  */
			"lcd Task.",		 /* Name to help debugging.     */
			&lcd_task,			 /* Pointer to the task's code. */
			DEF_NULL,			 /* Pointer to task's argument. */
			NORMAL_PRIORITY,	 /* Task's priority.            */
			&lcdSTK[0],			 /* Pointer to base of stack.   */
			(STACK_SIZES / 10u), /* Stack limit, from base.     */
			STACK_SIZES,		 /* Stack size, in CPU_STK.     */
			10u,				 /* Messages in task queue.     */
			120u,				 /* Round-Robin time quanta.    */
			DEF_NULL,			 /* External TCB data.          */
			OS_OPT_TASK_STK_CHK, /* Task options.               */
			&tskErr);
		if (semErr.Code || tmrErr.Code || tskErr.Code)
			EFM_ASSERT(false);

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

		/* Use Narrow font */
		GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

		/* Draw text on the memory lcd display*/
		GLIB_drawStringOnLine(&glibContext,
							  "Welcome to...\n**Wolfenstein Assault**!",
							  0,
							  GLIB_ALIGN_LEFT,
							  5,
							  5,
							  true);
		/* Post updates to display */
		DMD_updateDisplay();
	}

	void draw_sides(GLIB_Rectangle_t * pRectLeft, GLIB_Rectangle_t * pRectRight)
	{
		GLIB_drawRectFilled(&glibContext, pRectLeft);
		GLIB_drawRectFilled(&glibContext, pRectRight);
	}

#define RAILGUN_THICKNESS 3
#define RAILGUN_LENGTH 10

	void draw_plat(int x)
	{
		// Draw the platform
		GLIB_drawLine(&glibContext, x - PLATFORM_WIDTH / 2, PLATFORM_Y, x + PLATFORM_WIDTH / 2, PLATFORM_Y);

		// Calculate the angle of the railgun aiming at the satchel
		float angle = atan2f(SATCHEL_Y - PLATFORM_Y, SATCHEL_X - x);

		// Calculate the endpoint of the railgun
		int railgun_end_x = x + RAILGUN_LENGTH * cos(angle);
		int railgun_end_y = PLATFORM_Y - RAILGUN_LENGTH * sin(angle);

		// Draw the railgun
		for (int i = 0; i < RAILGUN_THICKNESS; i++)
		{
			GLIB_drawLine(&glibContext, x, PLATFORM_Y - i, railgun_end_x, railgun_end_y - i);
		}
	}

	void draw_laser(int x, int y)
	{
		GLIB_drawLine(&glibContext, x, y, x, y - LASER_LENGTH);
	}

	void draw_satchel(int x, int y)
	{
		GLIB_Rectangle_t satchelRect = {
			x - SATCHEL_PIXEL_SIZE / 2,
			y - SATCHEL_PIXEL_SIZE / 2,
			x + SATCHEL_PIXEL_SIZE / 2,
			y + SATCHEL_PIXEL_SIZE / 2};
		GLIB_drawRectFilled(&glibContext, &satchelRect);
	}

	static GLIB_Rectangle_t pRectLeft = {
		0,
		0,
		CANYON_START,
		SCREEN_PIXELS - 1};
	static GLIB_Rectangle_t pRectRight = {
		CANYON_END,
		0,
		SCREEN_PIXELS - 1,
		SCREEN_PIXELS - 1};
	static int charge_text_offset = -1 * CANYON_START - 5;
	static int lifeXOffset = CANYON_START + HM_PIXEL_RADIUS + 3;
	void draw_game(void)
	{
		RTOS_ERR mutErr;
		draw_sides(&pRectLeft, &pRectRight);

		// draw the platform
		OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
		if (mutErr.Code)
			EFM_ASSERT(false);
		int platX = (int)round(platform_data.x);
		OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutErr);
		if (mutErr.Code)
			EFM_ASSERT(false);
		draw_plat(platX);

		int lowestY = 0;
		int lowestX = 0;
		// draw the hm
		OSMutexPend(&hm_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
		if (mutErr.Code)
			EFM_ASSERT(false);
		for (int i = 0; i < HM_COUNT; i++)
		{
			draw_hm((int)round(HMs[i].x), (int)round(HMs[i].y));
			if (lowestY < (int)HMs[i].y)
			{
				lowestY = (int)HMs[i].y;
				lowestX = (int)HMs[i].x;
			}
		}
		OSMutexPost(&hm_mutex, OS_OPT_POST_NONE, &mutErr);
		if (mutErr.Code)
			EFM_ASSERT(false);

		// draw the shield
		if (shield_state.active)
		{
			GLIB_drawCircle(&glibContext, platX, PLATFORM_Y, (PLATFORM_WIDTH / 2) + 1);
		}

		// draw the cannon
		int xDist = CANYON_END - lowestX;
		int yDist = PLATFORM_Y - lowestY;
		int dist = sqrt((xDist * xDist) + (yDist * yDist));
		int x2 = CANYON_END - (xDist * CANNON_LENGTH / dist);
		int y2 = PLATFORM_Y - (yDist * CANNON_LENGTH / dist);
		for (int i = -5; i <= 5; i++)
		{
			GLIB_drawLine(&glibContext, CANYON_END, PLATFORM_Y + i, x2, y2 + i);
		}
		if (laser_fired > 0)
		{
			GLIB_drawLine(&glibContext, CANYON_END, PLATFORM_Y, shotX, shotY);
			laser_fired--;
		}
		char laser_charges_string[] = "Charges:  ";
		laser_charges_string[9] = '0' + laser_charges;
		GLIB_drawStringOnLine(
			&glibContext,
			laser_charges_string,
			12,
			GLIB_ALIGN_RIGHT,
			charge_text_offset,
			0,
			true);

		for (int i = 1; i <= lives; i++)
		{
			draw_hm(lifeXOffset, (HM_PIXEL_RADIUS + 1) * 2 * i);
		}
	}

	void draw_game_stopped()
	{
		
	}

	void lcd_task()
	{
		RTOS_ERR semErr;
		RTOS_ERR tmrErr;

		GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
		OSTmrStart(&lcdTmr, &tmrErr);
		if (tmrErr.Code != RTOS_ERR_NONE)
			EFM_ASSERT(false);

		while (1)
		{
			OSSemPend(&lcd_semaphore, 0, OS_OPT_PEND_BLOCKING, NULL, &semErr);
			if (semErr.Code)
				EFM_ASSERT(false);

			GLIB_clear(&glibContext);
			if (gameState == IN_PROGRESS)
			{
				draw_game();
			}
			else
			{
				draw_game_stopped();
			}
			DMD_updateDisplay();
		}
	}
