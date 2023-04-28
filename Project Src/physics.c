/*
 * physics.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "physics.h"

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
		if (mutexErr.Code)
			EFM_ASSERT(false);

		// Update satchel charges
		updateSatchelCharges(deltaTime);

		checkCollisions(Satchels);

		OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
		if (mutexErr.Code)
			EFM_ASSERT(false);

		// Update rail gun shots
		// updateRailGunShots(deltaTime);

		// Check for collisions and endgame
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

	plat_data->x += plat_data->vx * PHYSICS_DELTA;
	plat_data->vx += plat_data->ax * PHYSICS_DELTA;
	if ((plat_data->x - (PLATFORM_WIDTH / 2)) < CANYON_START)
	{
		if (plat_data->vx < (-1 * MAX_SPEED))
		{
			//			game_over("Too Fast");
		}
		else if (PLATFORM_BOUNCE_ENABLED)
		{
			plat_data->ax = 0;
			plat_data->vx = fabs(plat_data->vx);
		}
		else
		{
			plat_data->ax = 0;
			plat_data->vx = 0;
			plat_data->x = CANYON_START + PLATFORM_WIDTH / 2;
		}
	}
	else if ((plat_data->x + (PLATFORM_WIDTH / 2)) > CANYON_END)
	{
		if (plat_data->vx > MAX_SPEED)
		{
			//			game_over("Too Fast");
		}
		else if (PLATFORM_BOUNCE_ENABLED)
		{
			plat_data->ax = 0;
			plat_data->vx = -1 * fabs(plat_data->vx);
		}
		else
		{
			plat_data->ax = 0;
			plat_data->vx = 0;
			plat_data->x = CANYON_END - PLATFORM_WIDTH / 2;
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


// Check for collisions and endgame

void checkCollisions(struct SatchelData Satchels, struct PlatData platform_data,
					 struct ShieldState *shieldDat)
{
	RTOS_ERR mutexErr;
	for (int i = 0; i < SATCHEL_COUNT; i++)
	{
		if ((Satchels.y + GRAVITY_PIXELS) >= PLATFORM_Y && Satchels.vy > 0)
		{ // reached the platforms y
			if (Satchels.vy > GAME_OVER_SPEED && Satchels.x < (platform_data.x + (PLATFORM_WIDTH / 2)) && Satchels.x > (platform_data.x - (PLATFORM_WIDTH / 2)))
			{ // between the platform bounds

				// TODO
				Satchels.vy *=
					shieldDat->active ? ACTIVE_KINETIC_GAIN : PASSIVE_KINETIC_REDUCTION;
			}
			else
			{
				// TODO game over (respawning satchel is temporary for debugging)
				if (!auto_cannon || !shoot_laser(i))
				{
					// TODO
					decrement_life();

					OSMutexPend(&satchel_mutex, 0, OS_OPT_PEND_BLOCKING, NULL,
								&mutexErr);
					if (mutexErr.Code)
						EFM_ASSERT(false);

					generate_satchel(i);

					OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
					if (mutexErr.Code)
						EFM_ASSERT(false);
				}
			}
		}
		else if (Satchels.y < 0)
		{
			OSMutexPend(&satchel_mutex, 0, OS_OPT_PEND_BLOCKING, NULL,
						&mutexErr);
			if (mutexErr.Code)
				EFM_ASSERT(false);

			generate_satchel(i);

			OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
			if (mutexErr.Code)
				EFM_ASSERT(false);
			score++;
		}
	}
}

// Update rail gun shots
// void updateRailGunShots(deltaTime)
// {
// }
