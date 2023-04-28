/*
 * display.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "display.h"



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


	// Game start up
	GLIB_drawStringOnLine(&glibContext, "   Wolfenstein\n     Assault", 1,
			GLIB_ALIGN_LEFT, 0, 10,
			true);

	/* Draw text on the memory lcd display*/
	GLIB_drawStringOnLine(&glibContext, " Press any key \n  To start", 1,
			GLIB_ALIGN_LEFT, 0, 50,
			true);
	/* Post updates to display */
	DMD_updateDisplay();
}



void draw_sides()
{
	// GLIB_drawRectFilled(&glibContext, pRectLeft);
	// GLIB_drawRectFilled(&glibContext, pRectRight);

	// Draw castle
	GLIB_Rectangle_t castle_rect = {0, 28, 30, 28 + 15};
	GLIB_drawRectFilled(&glibContext, &castle_rect);

	GLIB_Rectangle_t castle_rect2 = {0, 28 + 5, 0 + 5, 128};
	GLIB_drawRectFilled(&glibContext, &castle_rect2);

	// Palf
	GLIB_Rectangle_t castle_rect3 = {59, 128 - 5, 59 + 20, 128};
	GLIB_drawRectFilled(&glibContext, &castle_rect3);

	// Gun
	GLIB_Rectangle_t castle_rect4 = {59 + 9, 128 - 15, 59 + 11, 128};
	GLIB_drawRectFilled(&glibContext, &castle_rect4);

	// Sheild
	GLIB_drawCircle(&glibContext, 59 + 10, 128 - 5, 15);

	// satchel Gun
}

#define RAILGUN_THICKNESS 3
#define RAILGUN_LENGTH 10

void draw_plat(int x)
{ // TODO
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
	// TODO
	GLIB_drawLine(&glibContext, railgun_end_x, railgun_end_y, x, y);

	// Lazer
	// GLIB_drawLine(&glibContext, 59 + 10, 128 - 15, 28 + 3, 100 + 3);
}

void draw_satchel(int x, int y)
{
	// satchel Gun

	// GLIB_Rectangle_t satchelRect = {
	// 	x - SATCHEL_PIXEL_SIZE / 2,
	// 	y - SATCHEL_PIXEL_SIZE / 2,
	// 	x + SATCHEL_PIXEL_SIZE / 2,
	// 	y + SATCHEL_PIXEL_SIZE / 2};

	GLIB_Rectangle_t satchelRect = {
		x,
		y,
		x + 5,
		y + 5};
	GLIB_drawRectFilled(&glibContext, &satchelRect);
}

// static GLIB_Rectangle_t pRectLeft = {
// 	0,
// 	0,
// 	CANYON_START,
// 	SCREEN_PIXELS - 1};
// static GLIB_Rectangle_t pRectRight = {
// 	CANYON_END,
// 	0,
// 	SCREEN_PIXELS - 1,
// 	SCREEN_PIXELS - 1};
static int charge_text_offset = -1 * CANYON_START - 5;
static int lifeXOffset = CANYON_START + HM_PIXEL_RADIUS + 3;

void draw_game(void)
{
	RTOS_ERR mutErr;

	// Draw the castle
	draw_sides();

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
	// draw the

	OSMutexPend(&sc_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
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
	OSMutexPost(&sc_mutex, OS_OPT_POST_NONE, &mutErr);
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

	// Update the Charging bar
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

	// Update the satchel
	// TODO
	for (int i = 1; i <= lives; i++)
	{
		draw_hm(lifeXOffset, (HM_PIXEL_RADIUS + 1) * 2 * i);
	}
}

void draw_game_stopped()
{
	// Game end here
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
