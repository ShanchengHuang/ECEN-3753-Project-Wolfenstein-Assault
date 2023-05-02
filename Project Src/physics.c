/*
 * physics.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "physics.h"

uint8_t railgun_shots = 5;
OS_SEM railgun_semaphore;
int railgun_fired = 0;
int shotX;
int shotY;

int win_time = 0;
bool castle_hit = 0;

void Physics_Task(void *p_arg)
{

	RTOS_ERR err;
	RTOS_ERR mutexErr;
	(void)&p_arg;

	while (1)
	{

		// Delay for milliseconds before next update
		OSTimeDlyHMSM(0, 0, 0, 15, OS_OPT_TIME_HMSM_STRICT, &err);

		OSMutexPend(&platform_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
		update_platform(&platform_data);
		OSMutexPost(&platform_mutex, OS_OPT_POST_NONE, &mutexErr);

		// Update satchel charges
		OSMutexPend(&satchel_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
		updateSatchelCharges(&Satchels);
		OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
		if (mutexErr.Code)
			EFM_ASSERT(false);

		// Update rail gun shots
		OSMutexPend(&railgun_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
		updateRailGunShots(&Bullet);
		OSMutexPost(&railgun_mutex, OS_OPT_POST_NONE, NULL, &mutexErr);

		// void updateRailGunShots(&shield_state)

		// Check for collisions and endgame
		checkBulletHit(Bullet);
		checkCollisions(Satchels, platform_data, shield_state);

		if (win_time == 10)
		{
			const char *game_stopped_message = "YOU WIN!!!";
			draw_game_stopped(game_stopped_message);
		}
	}
}

void Physics_Task_Create()
{
	RTOS_ERR err;

	OSTaskCreate(&Physics_TaskTCB,				/* Pointer to the task's TCB.  */
				 "Physics_Task.",				/* Name to help debugging.     */
				 &Physics_Task,					/* Pointer to the task's code. */
				 DEF_NULL,						/* Pointer to task's argument. */
				 Physics_Task_PRIO,				/* Task's priority.            */
				 &Physics_TaskStk[0],			/* Pointer to base of stack.   */
				 (Physics_Task_STK_SIZE / 10u), /* Stack limit, from base.     */
				 Physics_Task_STK_SIZE,			/* Stack size, in CPU_STK.     */
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

// ... other code ...

// void updatePlatformForce()
// {
// 	// Obtain the slider position from the CapSense input
// 	uint8_t sliderPosition = CAPSENSE_getSliderPosition();

// 	// Calculate the force based on the slider position
// 	int32_t force = ConfigurationData.Platform.MaxForce * (sliderPosition - 128) / 128;

// 	// Apply the force to the platform
// 	applyForceToPlatform(force);
// }

void update_platform(struct PlatData *plat_data)
{

	int MAX_SPEED_END = MAX_SPEED - 40;

	plat_data->x += plat_data->vx * PHYSICS_DELTA;
	plat_data->vx += plat_data->ax * PHYSICS_DELTA;

	if ((plat_data->x - (PLATFORM_WIDTH / 2)) < CANYON_START)
	{
		if (plat_data->vx < (-1 * MAX_SPEED_END))
		{
			const char *game_stopped_message = "Too Fast";
			draw_game_stopped(game_stopped_message);
		}
		else
		{
			plat_data->ax = 0;
			plat_data->vx = fabs(plat_data->vx);
		}
	}
	else if ((plat_data->x + (PLATFORM_WIDTH / 2)) > CANYON_END)
	{
		if (plat_data->vx > MAX_SPEED_END)
		{
			const char *game_stopped_message = "Too Fast";
			draw_game_stopped(game_stopped_message);
		}
		else
		{
			plat_data->ax = 0;
			plat_data->vx = -1 * fabs(plat_data->vx);
		}
	}
}

void updateSatchelCharges(struct SatchelData *Satchels)
{

	Satchels->x += Satchels->vx * PHYSICS_DELTA;
	Satchels->y += Satchels->vy * PHYSICS_DELTA;
	Satchels->vy += GRAVITY_PIXELS * PHYSICS_DELTA;

	if ((Satchels->x - SATCH_DIAMETER) < CANYON_START)
	{
		Satchels->vx = fabs(Satchels->vx);
	}
	else if ((Satchels->x + SATCH_DIAMETER) > CANYON_END)
	{
		Satchels->vx = -1 * fabs(Satchels->vx);
	}
}
//
//// Check for collisions and endgame

void checkCollisions(struct SatchelData Satchels, struct PlatData platform_data,
					 struct ShieldState shieldDat)
{
	RTOS_ERR mutexErr;

	if ((Satchels.y + GRAVITY_PIXELS) >= PLATFORM_Y && Satchels.vy > 0)
	{ // If reached the platforms
		if (Satchels.x < (platform_data.x + (PLATFORM_WIDTH / 2)) && Satchels.x > (platform_data.x - (PLATFORM_WIDTH / 2)))
		{ // with in  the platform and the shell is on

			if (shield_state.active)
			{
				OSMutexPend(&satchel_mutex, 0, OS_OPT_PEND_BLOCKING, NULL,
							&mutexErr);
				generate_satchel();
				OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
				if (mutexErr.Code)
					EFM_ASSERT(false);

				if (castle_hit)
				{
					win_time++;
				}
			}
			else
			{
				const char *game_stopped_message = "Hit by the satchel";
				draw_game_stopped(game_stopped_message);
			}
		}
	}
}

void checkBulletHit(struct BulletData Bullet)
{

	if (Bullet.x > 0 && Bullet.x < 30 && Bullet.y > 28 && Bullet.y < 43)
	{
		castle_hit = 1;
	}
}
// Update rail gun shots
void updateRailGunShots(struct BulletData *Bullet)
{
	Bullet->x += Bullet->vx * PHYSICS_DELTA;
	Bullet->y += Bullet->vy * PHYSICS_DELTA;
	Bullet->vy += GRAVITY_PIXELS * PHYSICS_DELTA;

	if ((Bullet->x - SATCH_DIAMETER) < CANYON_START)
	{
		Bullet->vx = fabs(Bullet->vx);
	}
	else if ((Bullet->x + SATCH_DIAMETER) > CANYON_END)
	{
		Bullet->vx = -1 * fabs(Bullet->vx);
	}
}
