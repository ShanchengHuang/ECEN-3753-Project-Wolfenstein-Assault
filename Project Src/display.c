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

OS_TCB LCDOutputTCB; /*   Task Control Block.   */
CPU_STK LCDOutputStk[LCDOutput_SIZE]; /*   Stack.                */

// extern uint8_t SS_COUNT;
// extern struct SatchelData satchels;
// extern OS_MUTEX ss_mutex;
extern OS_MUTEX platform_mutex;

// extern struct PlatformData platform_data;
extern struct ShieldState shield_state;
// extern uint8_t railgun_charges;

extern int cursor_pos;
extern int PLATFORM_BOUNCE_ENABLED;
extern int score;
extern int high_score;
extern int lives;
extern int railgun_fired;
// extern char death_cause[];
extern int shotX;
extern int shotY;

void lcd_timer_cb(void) {
	RTOS_ERR semPostErr;
	OSSemPost(&lcd_semaphore,
	OS_OPT_POST_1, &semPostErr);
	if (semPostErr.Code)
		EFM_ASSERT(false);
}

void LCD_init() {
	RTOS_ERR semErr;
	RTOS_ERR tmrErr;
	RTOS_ERR tskErr;
	OSSemCreate(&lcd_semaphore, "lcd Semaphore", 0, &semErr);

	OSTmrCreate(&lcdTmr, "lcd timer", 0,
	LCD_PERIOD,
	OS_OPT_TMR_PERIODIC, &lcd_timer_cb,
	NULL, &tmrErr);

	OSTaskCreate(&lcdTCB, /* Pointer to the task's TCB.  */
	"lcd Task.", /* Name to help debugging.     */
	&lcd_task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	NORMAL_PRIORITY, /* Task's priority.            */
	&lcdSTK[0], /* Pointer to base of stack.   */
	(STACK_SIZES / 10u), /* Stack limit, from base.     */
	STACK_SIZES, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	120u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
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
	GLIB_setFont(&glibContext, (GLIB_Font_t*) &GLIB_FontNarrow6x8);

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

void draw_sides() {

	// Draw castle
	GLIB_Rectangle_t castle_rect = { 0, 28, 30, 28 + 15 };
	GLIB_drawRectFilled(&glibContext, &castle_rect);

	GLIB_Rectangle_t castle_rect2 = { 0, 28 + 5, 0 + 5, 128 };
	GLIB_drawRectFilled(&glibContext, &castle_rect2);

	//	// Palf
	//	GLIB_Rectangle_t castle_rect3 = { 59, 128 - 5, 59 + 20, 128 };
	//	GLIB_drawRectFilled(&glibContext, &castle_rect3);
	//
	//	// Gun
	//	GLIB_Rectangle_t castle_rect4 = { 59 + 9, 128 - 15, 59 + 11, 128 };
	//	GLIB_drawRectFilled(&glibContext, &castle_rect4);
	//
	//	// Sheild
	//	GLIB_drawCircle(&glibContext, 59 + 10, 128 - 5, 15);

	// satchel Gun
}

#define RAILGUN_THICKNESS 3
#define RAILGUN_LENGTH 10

void draw_plat(int x) { // TODO

	GLIB_Rectangle_t castle_rect3 = { x - 5, 128 - 5, x + 20 - 5, 128 };
	GLIB_drawRectFilled(&glibContext, &castle_rect3);
}

void draw_satchel(int x, int y) {
	GLIB_Rectangle_t satchelRect = { x, y, x + 5, y + 5 };
	GLIB_drawRectFilled(&glibContext, &satchelRect);
}

void draw_bullet(int x, int y) {
	GLIB_drawCircleFilled(&glibContext, x, y, 4);
}

void draw_game(void) {
	RTOS_ERR mutErr;

	// Draw the castle
	draw_sides();

	// draw the platform
	OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
	if (mutErr.Code)
		EFM_ASSERT(false);

	int platX = (int) round(platform_data.x);

	OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutErr);
	if (mutErr.Code)
		EFM_ASSERT(false);
	draw_plat(platX);

	//	int lowestY = 0;
	//	int lowestX = 0;
	// draw the
	OSMutexPend(&satchel_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
	int sc_x = Satchels.x;
	int sc_y = Satchels.y;
	OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutErr);
	draw_satchel(sc_x, sc_y);

	OSMutexPend(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutErr);
	int bt_x = Bullet.x;
	int bt_y = Bullet.y;
	OSMutexPost(&railgun_mutex, OS_OPT_POST_NONE, &mutErr);
	if (railgun_fired == 1) {
		draw_bullet(bt_x, bt_y);
	}

	// draw the shield
//	if (shield_state.active) {
//		GLIB_drawCircle(&glibContext, platX, PLATFORM_Y,
//				(PLATFORM_WIDTH / 2) + 1);
//	}

	//	//Charge
	//	GLIB_Rectangle_t castle_rect6 = { 0, 10, 50, 10 + 5 };
	//	GLIB_drawRect(&glibContext, &castle_rect6);
	//
	//	GLIB_Rectangle_t castle_rect7 = { 0, 10, 30, 10 + 5 };
	//	GLIB_drawRectFilled(&glibContext, &castle_rect7);
	//
	//	GLIB_drawStringOnLine(&glibContext, "Charging", 0, GLIB_ALIGN_LEFT, 0, 0,
	//	true);


	//	//Time left
	int left = 10 - win_time;
	char c[10];
	sprintf(c, "%d", left);
	GLIB_drawStringOnLine(&glibContext, c, 0, GLIB_ALIGN_RIGHT, 0, 10, true);

	//	GLIB_drawStringOnLine(&glibContext, "Second", 0, GLIB_ALIGN_RIGHT, 0, 0,
	//	true);

}



void draw_game_stopped(const char *x) {
	RTOS_ERR err;
	GLIB_clear(&glibContext);

	GLIB_drawStringOnLine(&glibContext, x, 4, GLIB_ALIGN_CENTER, 0, 0,
	true);

	DMD_updateDisplay();

//	OSTimeDlyHMSM(1, 0, 0, 0, OS_OPT_TIME_HMSM_STRICT, &err);

	game_over();
}

void lcd_task() {
	RTOS_ERR semErr;
	//	RTOS_ERR tmrErr;

	GLIB_setFont(&glibContext, (GLIB_Font_t*) &GLIB_FontNormal8x8);

	//	OSTmrStart(&lcdTmr, &tmrErr);
	//	if (tmrErr.Code != RTOS_ERR_NONE)
	//		EFM_ASSERT(false);

	// Delate for awakened periodically

	while (1) {

		OSTimeDlyHMSM(0, 0, 0, 15, OS_OPT_TIME_HMSM_STRICT, &semErr);

		GLIB_clear(&glibContext);

		draw_game();

		DMD_updateDisplay();

	}
}
