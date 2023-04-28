/*
 * physics.c
 *
 *  Created on: 7 Apr 2023
 *      Author: shancheng
 */

#include "physics.h"

void Physics_Task(void *p_arg) {

	RTOS_ERR err;
	RTOS_ERR mutexErr;
	(void) &p_arg;

	while (1) {

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

		checkCollisions();

		OSMutexPost(&satchel_mutex, OS_OPT_POST_NONE, &mutexErr);
		if (mutexErr.Code)
			EFM_ASSERT(false);

		// Update rail gun shots
		// updateRailGunShots(deltaTime);

		// Check for collisions and endgame
	}

}

void Physics_Task_Create() {
	RTOS_ERR err;

	OSTaskCreate(&Physics_TaskTCB, /* Pointer to the task's TCB.  */
	"Physics_Task.", /* Name to help debugging.     */
	&Physics_Task, /* Pointer to the task's code. */
	DEF_NULL, /* Pointer to task's argument. */
	Physics_Task_PRIO, /* Task's priority.            */
	&Physics_TaskStk[0], /* Pointer to base of stack.   */
	(Physics_Task_STK_SIZE / 10u), /* Stack limit, from base.     */
	Physics_Task_STK_SIZE, /* Stack size, in CPU_STK.     */
	10u, /* Messages in task queue.     */
	0u, /* Round-Robin time quanta.    */
	DEF_NULL, /* External TCB data.          */
	OS_OPT_TASK_STK_CHK, /* Task options.               */
	&err);

	if (err.Code != RTOS_ERR_NONE) {
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

void update_platform(SharedData *shared_data) {

	shared_data->xPos += shared_data->xVel * PHYSICS_DELTA;
	shared_data->xVel += shared_data->capSenseForce * PHYSICS_DELTA;

	float half_platform_length = default_config.platform.length / 2;

	if ((shared_data->xPos - half_platform_length) < CANYON_START) {
		if (shared_data->xVel
				< (-1 * default_config.platform.max_platform_bounce_speed)) {
			// TODO game over
		} else if (PLATFORM_BOUNCE_ENABLED) {
			shared_data->capSenseForce = 0;
			shared_data->xVel = fabs(shared_data->xVel);
		} else {
			shared_data->capSenseForce = 0;
			shared_data->xVel = 0;
			shared_data->xPos = CANYON_START + half_platform_length;
		}
	} else if ((shared_data->xPos + half_platform_length) > CANYON_END) {
		if (shared_data->xVel
				> default_config.platform.max_platform_bounce_speed) {
			// TODO game over
		} else if (PLATFORM_BOUNCE_ENABLED) {
			shared_data->capSenseForce = 0;
			shared_data->xVel = -1 * fabs(shared_data->xVel);
		} else {
			shared_data->capSenseForce = 0;
			shared_data->xVel = 0;
			shared_data->xPos = CANYON_END - half_platform_length;
		}
	}
}

void updateSatchelCharges(struct HoltzmanData Satchels[]) {
	for (int i = 0; i < SATCHEL_COUNT; i++) {
		Satchels[i].x += Satchels[i].vx * PHYSICS_DELTA;
		Satchels[i].y += Satchels[i].vy * PHYSICS_DELTA;
		Satchels[i].vy += GRAVITY_PIXELS * PHYSICS_DELTA;
		if ((Satchels[i].x - SATCH_DIAMETER) < CANYON_START) {
			Satchels[i].vx = fabs(Satchels[i].vx);
		} else if ((Satchels[i].x + SATCH_DIAMETER) > CANYON_END) {
			Satchels[i].vx = -1 * fabs(Satchels[i].vx);
		}
	}
}

// Check for collisions and endgame

void checkCollisions(struct SatchelData Satchels[],
		struct SharedData *shared_data, struct ShieldState *shieldDat) {
	RTOS_ERR mutexErr;
	for (int i = 0; i < SATCHEL_COUNT; i++) {
		if ((Satchels[i].y + GRAVITY_PIXELS) >= PLATFORM_Y
				&& Satchels[i].vy > 0) { // reached the platforms y
			if (Satchels[i].vy > GAME_OVER_SPEED
					&& Satchels[i].x < (shared_data->x + (PLATFORM_WIDTH / 2))
					&& Satchels[i].x
							> (shared_data->x - (PLATFORM_WIDTH / 2))) { // between the platform bounds
																		 // TODO
				Satchels[i].vy *=
						shieldDat->active ?
								ACTIVE_KINETIC_GAIN : PASSIVE_KINETIC_REDUCTION;
			} else {
				// TODO game over (respawning satchel is temporary for debugging)
				if (!auto_cannon || !shoot_laser(i)) {
					// TODO
					decrement_life();

					OSMutexPend(&sc_mutex, 0, OS_OPT_PEND_BLOCKING, NULL,
							&mutexErr);
					if (mutexErr.Code)
						EFM_ASSERT(false);
					generate_satchel(i);
					OSMutexPost(&sc_mutex, OS_OPT_POST_NONE, &mutexErr);
					if (mutexErr.Code)
						EFM_ASSERT(false);
				}
			}
		} else if (Satchels[i].y < 0) {
			OSMutexPend(&sc_mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &mutexErr);
			if (mutexErr.Code)
				EFM_ASSERT(false);

			generate_satchel(i);

			OSMutexPost(&sc_mutex, OS_OPT_POST_NONE, &mutexErr);
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
